// Omnet++ model of an apec bufferless noc.
// Copyright (C) 2019  by the author(s)
// 
// This program is free software: you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software
// Foundation, either version 3 of the License, or (at your option) any later
// version.
// 
// This program is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for mor
// details.
// 
// You should have received a copy of the GNU General Public License along with
// this program.  If not, see <https://www.gnu.org/licenses/>.
//
// File      : decoder.cc
// Created   : 21 Mar 2017 for apec_bless_noc
// Author(s) : Michael Vonbun
// E-mail(s) : michael.vonbun@tum.de
// 

// stl
#include <algorithm> // std::find
#include <cassert>   // use for debug
#include <cstdio>    // sprintf, printf
#include <cstring>   // strcmp
#include <fstream>   // std::ifstream
#include <iostream>  // std::cout
#include <iterator>  // std::istream_iterator
#include <limits>    // out-of-bounds checking
#include <stdexcept> // std::runtime_error

// project utilities

// project messages
#include "add_module_statistic.h"
#include "debug_messages.h"
#include "enum_definitions_m.h"
#include "enum_utilities.h"
#include "noc_flits.h"

// module
#include "decoder_housekeepers.h"
#include "decoding_state.h"

// header
#include "decoder.h"

#define LOADPATH "../src/codes/"

// pure virtual base class; therefore, no Omnet Define_Module()
Decoder::Decoder()
    : m_id{0}, m_t_clk{0}, m_num_nodes{0}, m_router_release_mode{0},
      m_ack_mode{0}, m_tail_event{nullptr}, m_instant_tail_event{nullptr},
      m_flit{nullptr}, m_flit_is_head{false}, m_flit_is_tail{false},
      m_housekeeper{nullptr}, m_ack_delay{0}
{
}

Decoder::~Decoder()
{
    for (size_t i = 0; i < m_head.size(); ++i) {
        delete m_head.top();
        m_head.pop();
    }
    cancelAndDelete(m_tail_event);
    cancelAndDelete(m_instant_tail_event);
}

void Decoder::initialize()
{
    // get static module parameters from ned
    m_id = par("id").longValue();
    m_t_clk = par("t_clk").doubleValue();
    m_num_nodes = par("num_nodes").longValue();
    m_router_release_mode =
        util::convertResourceReleaseType(par("release_mode").stringValue());
    m_ack_mode = util::convertAckType(par("ack_mode").stringValue());
    m_aggregate_en = par("aggregate_en").boolValue();

    // Self-Message for calling handleEnd() in LengthMode
    m_tail_event = new cMessage;
    m_tail_event->setKind(RELEVENT);
    m_tail_event->setSchedulingPriority(SP_LAST);

    m_instant_tail_event = new cMessage;
    m_instant_tail_event->setKind(RELEVENT);
    m_instant_tail_event->setSchedulingPriority(SP_VERY_LATE);

    // reset members
    m_housekeeper = newHousekeeper(m_num_nodes);

    // register statistic signals
    // clang-format off
    m_signal_num_flit = util::addModuleStatistic(
       this, "dec-", m_id, "_num_flit", "dec_num_flit");
    m_signal_num_flit_rx_before = util::addModuleStatistic(
       this, "dec-", m_id, "_num_flit_rx_before", "dec_num_flit_rx_before");
    // clang-format on

    childInitialize();

// debug
#ifdef DEBUGENABLE
    if (m_id == m_debug_enable) {
        m_file.open("debug.txt");
        m_file << "t | id | treeId | kind | seqNum | stack\n";
        m_file.flush();
    }
#endif
}

void Decoder::finish()
{
// debug
#ifdef DEBUGENABLE
    if (m_id == m_debug_enable) {
        m_file.close();
    }
#endif
}

// deal with incoming messages
void Decoder::handleMessage(cMessage *msg)
{
// add validation code here
#ifdef VALIDATE
    if (!msg->isSelfMessage()) {
        std::cout << "t=" << simTime();
        printf("\t%10s%-10s\t kind=%2d\t msgid=%-8ld",
               this->getParentModule()->getFullName(), ".decoder",
               msg->getKind(), msg->getId());
        std::cout << std::endl;
    }
#endif

// add debug code here
#ifdef DEBUGENABLE
    if (msg->getId() == DEBUGMSGID) {
        std::cout << this->getFullPath() << ":" << simTime() << std::endl;
        EV_DEBUG << "!!!\n";
    }
#endif

// debug
#ifdef DEBUGENABLE
    if (m_id == m_debug_enable) {
        m_file << simTime() << " | " << msg->getId() << " | "
               << msg->getTreeId() << " | " << msg->getKind() << "  "
               << " | " << check_and_cast<RootFlit *>(msg)->getSequence_number()
               << "     "
               << " | "
               << ((m_was_received.empty())
                       ? ("empty")
                       : (((m_was_received.top()) ? ("true") : ("false"))));
        m_file.flush();
    }
#endif

    if (msg->isSelfMessage()) {
        // events (should only happen in Length Release Mode)
        EV_INFO << TAGRX << "self-message\n";
        if (m_router_release_mode == FLITCOUNT) {
            if (msg->getKind() == RELEVENT) {
                handleEnd();
            } else {
                assert(false && "DEC: received non-release event");
            }

        } else {
            assert(false &&
                   "DEC: received event but release mode is not length");
        }

    } else if (msg->arrivedOn("port$i")) {
        // message arrived from network interface
        handleFlit(msg);
    }

// debug
#ifdef DEBUGENABLE
    if (m_id == m_debug_enable) {
        m_file << " - "
               << ((m_was_received.empty())
                       ? ("empty")
                       : (((m_was_received.top()) ? ("true") : ("false"))))
               << "\n";
        m_file.flush();
    }
#endif
}

void Decoder::handleFlit(cMessage *msg)
{
    m_flit = check_and_cast<RootFlit *>(msg);
    m_flit_is_head = util::isHead(msg);
    m_flit_is_tail = util::isTail(msg, true);

    // setup decoding and push to stacks
    if (m_flit_is_head) {
        handleHead(msg);
    }

// debug
#ifdef DEBUGENABLE
    if (m_id == m_debug_enable) {
        m_file << " - "
               << ((m_was_received.empty())
                       ? ("empty")
                       : (((m_was_received.top()) ? ("true") : ("false"))));
        m_file.flush();
    }
#endif

    // process flits if the packet has not been received already
    if (!m_was_received.top()) {
        process();
    }

    deleteFlit();

    // reset decoding and pop from stacks
    if (m_flit_is_tail && m_router_release_mode == TAILFLAG) {
        handleEnd();
    }
}

/// Handle head flits.
/// Cast message, extract properties, reset member variables, and process flit.
void Decoder::handleHead(cMessage *msg)
{
    // store head and its properties
    m_head.push(check_and_cast<HeadFlit *>(msg));
    EV_DEBUG << TAGRX << "new " << PACKET(m_head.top()) << " head\n";

    // record all packets by emitting their flit numbers
    emit(m_signal_num_flit, m_head.top()->getNum_flit());

    // check if packet was (successfully) received already
    m_was_received.push(m_housekeeper->wasReceived(m_head.top()));

    if (m_was_received.top()) {
        EV_DEBUG << PACKET(m_head.top()) << "has been received already\n";
        if (m_ack_mode == ACK) {
            sendAck(0); // send ack back immediately
        }

        // record useless packets by emitting their flit numbers
        emit(m_signal_num_flit_rx_before, m_head.top()->getNum_flit());
        m_head.pop(); // discard current head (will be deleted using m_flit)

    } else {
        EV_DEBUG << TAGBEG << "decoding of " << PACKET(m_head.top()) << "\n";
        getHeadProperties(); // extract header properties

        // reset members
        processNewHead();

        if (m_router_release_mode == FLITCOUNT) {
            // Tail might get lost, schedule End
            int cycles_until_tailevent = m_head.top()->getNum_flit() - 1;
            if (cycles_until_tailevent == 0)
                scheduleAt(simTime(), m_instant_tail_event);
            else {
                scheduleAt(simTime() + m_t_clk * cycles_until_tailevent,
                           m_tail_event);
            }
        }
    }
}

// handle tail flit
void Decoder::handleEnd()
{
    // if packet has not been received already
    if (!m_was_received.top()) {
        // if true, Tail got lost and actionOnFailure() wasn't called
        if (m_is_enabled.top() == true)
            actionOnFailure();

        if (m_router_release_mode == FLITCOUNT && m_failure.top() == true) {
            EV_DEBUG << TAGDEL << "currently decoded packet header\n";
            delete m_head.top();
            m_head.pop();
        }

        // record statistics if packet has not been received already
        recordStatistic();

        // pop stack variables
        popStack();
    }
    m_was_received.pop();
}

void Decoder::popStack()
{
    m_num_flit.pop();
    m_num_baseflit.pop();
    m_num_codeflit.pop();

    //    m_decoding_state.pop();
    m_is_enabled.pop();
    m_is_decodable.pop();
    m_failure.pop();

    m_head_forward_delay.pop();
}

void Decoder::deleteFlit()
{
    // delete everything if packet has been received already
    if (m_was_received.top()) {
        delete m_flit;

    } else {
        // delete bodies and tails always
        if (!m_flit_is_head) {
            delete m_flit;
        }
        // but headers only if decoding failed,
        // which is set when tail is received and processed
        if (m_failure.top() && m_router_release_mode == TAILFLAG) {
            EV_DEBUG << TAGDEL << "currently decoded packet header\n";
            delete m_head.top();
            m_head.pop();
        }
    }
}

// extract header properties and store in private member variables
void Decoder::getHeadProperties()
{
    m_num_flit.push(m_head.top()->getNum_flit());
    m_num_baseflit.push(m_head.top()->getNum_baseflit());
    m_num_codeflit.push(m_head.top()->getNum_codeflit());
}

void Decoder::processNewHead()
{
    m_is_enabled.push(true); // enable decoder

    m_is_decodable.push(false); // reset packet valid state
    m_failure.push(false);      // reset end of decoding::failure state

    setCodeDelays(); // set decoder specific delays
}

// common processing for all flits of all codes
void Decoder::process()
{
    EV_INFO << TAGDEC;
    EV_INFO << ((m_flit_is_head) ? ("head") : (""));
    EV_INFO << ((m_flit->getKind() == BODY) ? ("body") : (""));
    EV_INFO << ((m_flit_is_tail) ? ("tail") : (""));
    EV_INFO << "<" << m_flit->getSequence_number() << ">\n";

    // if decoder is enabled, use current flit for decoding
    if (m_is_enabled.top()) {
        // if packet is not valid already, check if it becomes valid
        // after using this current flit for decoding
        if ((!m_is_decodable.top())) {
            decode(m_flit);
            m_is_decodable.top() = checkDecodability();
        }

        // if the packet is decodable now:
        //  forward head, and disable decoder; in ACK mode, send ACK
        // if not:
        //   if the last flit is received of
        if (m_is_decodable.top()) {
            EV_INFO << TAGSUCC << "at flit<"
                    << 1 + m_flit->getSequence_number();
            EV_INFO << " of " << m_num_baseflit.top() + m_num_codeflit.top()
                    << ">\n";
            actionOnSuccess();
        } else if (m_flit_is_tail) {
            EV_INFO << TAGFAIL << "decode " << PACKET(m_head.top()) << "\n";
            actionOnFailure();
        }
    }
}

/// Actions to take on decoding success.
void Decoder::actionOnSuccess()
{
    m_is_enabled.top() = false; // disable decoder
    m_failure.top() = false;    // decoder succeeded

    // in ACK mode, generate and send ACK back to ni
    if (m_ack_mode == ACK) {
        sendAck();
    }

    // mark packet as 'already received'
    m_housekeeper->updateReceived(m_head.top());
    forwardHead(); // forward header to sink
}

/// Actions to take on decoding failure.
void Decoder::actionOnFailure()
{
    m_is_enabled.top() = false; // disable decoder
    m_failure.top() = true;     // decoder failed

    // in NACK mode, generate and send NACK back to ni
    if (m_ack_mode == NACK) {
        sendNack();
    }
}

/// Default delay wrapper.
void Decoder::sendAck() { sendAck(m_ack_delay); }

/// Generate and send ACK for incoming packet.
void Decoder::sendAck(int ack_delay)
{
    // use header to generate ack and send it back to ni
    EV_INFO << TAGGEN << "ack for " << PACKET(m_head.top()) << "\n";
    //    AckFlit *ack = new AckFlit(m_head.top(), ACK);

    EV_INFO << TAGTX << "ack with delay of " << ack_delay << " cycles \n";
    sendDelayed(new AckFlit(m_head.top(), ACK), ack_delay * m_t_clk, "port$o");
}

/// Generate and send NACK if decoding fails.
void Decoder::sendNack()
{
    // use header to generate nack and send it back to ni
    EV_INFO << TAGGEN << "nack for " << PACKET(m_head.top()) << "\n";
    send(new NackFlit(m_head.top(), NACK), "port$o");
}

/// Forward header to sink.
void Decoder::forwardHead()
{
    EV_INFO << TAGTX << "forward head with delay of "
            << m_head_forward_delay.top() << " cycles\n";
    sendDelayed(m_head.top(), m_head_forward_delay.top() * m_t_clk, "out");
    m_head.pop();
}

/** Derived decoders. */
/** Uncoded decoder. */
Define_Module(UncodedDecoder);
UncodedDecoder::~UncodedDecoder() { delete m_housekeeper_impl; }

void UncodedDecoder::childInitialize()
{
    m_ack_delay_policy = par("ack_delay_policy").longValue();
    m_latency_policy = par("latency_policy").longValue();
}

BaseDecoderHousekeeper *UncodedDecoder::newHousekeeper(const int num_nodes)
{
    m_housekeeper_impl = new BaseDecoderHousekeeper(num_nodes);
    return m_housekeeper_impl;
}

void UncodedDecoder::setCodeDelays()
{
    // ACK transmission policy
    if (m_ack_delay_policy < 0) {
        // send ack after complete packet was received
        m_ack_delay = m_num_baseflit.top() - 1;
    } else {
        // if m_ack_delay_policy == 0: send ack as soon as possible
        // if m_ack_delay_policy > 0:  send ack after n cycles (parameter)
        m_ack_delay = m_ack_delay_policy;
    }

    // HEAD forward policy (sink calculates latency upon HEAD reception)
    m_head_forward_delay.push(0);
    if (m_latency_policy < 1) {
        // forward asap (non-application view)
        m_head_forward_delay.push(0);
    } else {
        // forward when entire packet was received (application view)
        m_head_forward_delay.push(m_num_baseflit.top() - 1);
    }
}

void UncodedDecoder::decode(RootFlit *flit)
{
    EV_INFO << TAGUSE << "uncoded decoder\n";
}

///  Uncoded packets are always valid as there should not be an error process.
bool UncodedDecoder::checkDecodability() { return true; }

void UncodedDecoder::popStack() { Decoder::popStack(); }

/// Uncoded decoder does not add other statistics.
void UncodedDecoder::recordStatistic() {}

/** Simple decoder. */
Define_Module(SimpleDecoder);
SimpleDecoder::SimpleDecoder(){};

SimpleDecoder::~SimpleDecoder() { delete m_housekeeper_impl; }

void SimpleDecoder::childInitialize()
{
    // register statistic signals
    // clang-format off
    m_signal_num_flit_received = util::addModuleStatistic(
        this, "dec-", m_id, "_num_flit_received", "dec_num_flit_received");
    m_signal_num_flit_needed = util::addModuleStatistic(
            this, "dec-", m_id, "_num_flit_needed", "dec_num_flit_needed");
    m_signal_nondecodable_num_flit = util::addModuleStatistic(
        this, "dec-", m_id, "_num_flit_nondecodable", "dec_num_flit_nondecodable");
    m_signal_num_flit_missing = util::addModuleStatistic(
        this, "dec-", m_id, "_num_flit_missing", "dec_num_flit_missing");
    // clang-format on
}

BaseDecoderHousekeeper *SimpleDecoder::newHousekeeper(const int num_nodes)
{
    m_housekeeper_impl =
        new CodedDecoderHousekeeper(num_nodes, !m_aggregate_en);
    return m_housekeeper_impl;
}

void SimpleDecoder::processNewHead()
{
    Decoder::processNewHead();
    // load decoding state
    m_decoding_state.push(m_housekeeper_impl->getState(m_head.top()));
}

void SimpleDecoder::setCodeDelays()
{
    // send ack as soon as possible
    m_ack_delay = 0;
    // head is only forwarded if enough packets arrived => delay is 0
    m_head_forward_delay.push(0);
}

/// Register flits sequence number in receive bitvector.
void SimpleDecoder::decode(RootFlit *flit)
{
    m_decoding_state.top()->setBit(flit->getSequence_number());
}

/// Packet is decodable if enough packets arrived.
/// Use hamming weight to check the number of received flits.
bool SimpleDecoder::checkDecodability()
{
    EV_INFO << TAGUSE << m_codetype << " decoder\n";
    EV_DEBUG << TAGDBG << "Check Decodability: need " << m_num_baseflit.top()
             << " flits for success - got "
             << m_decoding_state.top()->getWeight() << " so far\n";
    return !(m_decoding_state.top()->getWeight() < m_num_baseflit.top());
}

void SimpleDecoder::popStack()
{
    Decoder::popStack();
    m_decoding_state.pop();
}

void SimpleDecoder::recordStatistic()
{
    uint num_flit_received = m_decoding_state.top()->getWeight();
    emit(m_signal_num_flit_received, num_flit_received);
    emit(m_signal_num_flit_needed, m_num_baseflit.top());

    // record missing
    int num_missing_flits = (m_num_baseflit.top() - num_flit_received);
    if (num_missing_flits > 0) {
        emit(m_signal_nondecodable_num_flit, m_num_flit.top());
        emit(m_signal_num_flit_missing, num_missing_flits);
    }
}

/** Aggregate decoder. */
AggregateSimpleDecoder::~AggregateSimpleDecoder() { delete m_ext_housekeeper; }

BaseDecoderHousekeeper *
AggregateSimpleDecoder::newHousekeeper(const int num_nodes)
{
    m_ext_housekeeper = new AggregateCodedDecoderHousekeeper(num_nodes);
    return m_ext_housekeeper;
}

/// Actions to take if decoding failed.
// void AggregateSimpleDecoder::actionOnFailure()
//{
//    SimpleDecoder::actionOnFailure();
//    m_ext_housekeeper->saveState(m_head.top(), m_decoding_state.top());
//}

/** Cyclic decoder. */
Define_Module(CyclicDecoder);
Define_Module(AggregateCyclicDecoder);

/** Interleaved Block XOR decoder. */
Define_Module(InterleavedBlockXDecoder);
void InterleavedBlockXDecoder::childInitialize()
{
    SimpleDecoder::childInitialize();
    m_blocksize = par("blocksize").longValue();
}

bool InterleavedBlockXDecoder::checkDecodability()
{
    bool is_decodable = SimpleDecoder::checkDecodability();
    if (is_decodable) {
        EV_DEBUG << TAGDBG
                 << "Packet hamming weight is ok, check flit-blocks\n";

        // process all blocks (number of blocks equals the number of code flits)
        for (int i = 0; i < m_num_codeflit.top(); i++) {
            is_decodable = isBlockDecodable(i, !(i < m_num_codeflit.top() - 1));
            if (!is_decodable) {
                EV_DEBUG << TAGDBG << "Block " << i << " not decodable\n";
                break;
            }
            EV_DEBUG << TAGDBG << "Block " << i << " decodable\n";
        }
    }
    return is_decodable;
}

bool InterleavedBlockXDecoder::isBlockDecodable(int block_index,
                                                bool is_last_block)
{
    // offset is for head, then blocksize is increased by one due to
    // appended code flit
    int block_start_idx = 1 + block_index * (m_blocksize + 1);

    // number of elements in a block is either the blocksize + 1 for full blocks
    //   (blocksize payload flits + 1 code flit)
    // or the remainder of the number of flits that are used for encoding modulo
    //   the blocksize + code
    int num_elements_in_block = getNumberOfElementsOfBlock(is_last_block);
    int block_end_idx = block_start_idx + num_elements_in_block;

    EV_DEBUG << TAGDBG << "Checking block " << block_index << " with "
             << num_elements_in_block << " elements\n";

    int block_weight = 0;
    for (int i = block_start_idx; i < block_end_idx; i++) {
        block_weight += m_decoding_state.top()->getBit(i);
    }

    return !(block_weight < (num_elements_in_block - 1));
}

int InterleavedBlockXDecoder::getNumberOfElementsOfBlock(bool is_last_block)
{
    if (!is_last_block) {
        return m_blocksize + 1;
    } else {
        int num_payload_flit_to_decode;
        if (m_router_release_mode == TAILFLAG) {
            num_payload_flit_to_decode = m_num_flit.top() - 2;
        } else {
            num_payload_flit_to_decode = m_num_flit.top() - 1;
        }
        return num_payload_flit_to_decode % (m_blocksize + 1);
    }
}

/** Aggregate block-wise XOR decoder. */
Define_Module(AggregateInterleavedBlockXDecoder);
AggregateInterleavedBlockXDecoder::~AggregateInterleavedBlockXDecoder()
{
    delete m_ext_housekeeper;
}

BaseDecoderHousekeeper *
AggregateInterleavedBlockXDecoder::newHousekeeper(const int num_nodes)
{
    m_ext_housekeeper = new AggregateCodedDecoderHousekeeper(num_nodes);
    return m_ext_housekeeper;
}

/** Dynamic decoder.
 *
 */
Define_Module(DynamicDecoder);
Define_Module(AggregateDynamicDecoder);

/** FEC decoder.
 *
 */
Define_Module(FecDecoder);
Define_Module(AggregateFecDecoder);

/** Heuristic base decoder.
 *
 */
// read syndrome vector from file
std::vector<unsigned int>
HeuristicBaseDecoder::readSyndromeFromFile(const char *number_format,
                                           const char *comment_start)
{
    // get filename to read data
    char filename[100];
    sprintf(filename, "%s%s/%02d_%02d_%s.dat", LOADPATH, m_codetype.c_str(),
            m_num_baseflit.top(), m_num_codeflit.top(), "dec");

    // open and validate file stream
    std::ifstream fid(filename);
    std::string header;
    if (fid.is_open()) {
        std::getline(fid, header); // read in first line
    } else {
        char error_msg[200];
        sprintf(error_msg, "%s%s%s", "[dec] ", LOCATION("No such file: "),
                filename);
        throw std::runtime_error(error_msg);
    }

    // check if first line was a header line
    bool has_header = true;
    for (size_t i = 0; i < strlen(comment_start); ++i) {
        has_header &= (header[i] == comment_start[i]);
    }

    if (has_header) {
        // std::cout << "-> header found, was:" << std::endl;
        // std::cout << header << std::endl;
    } else {
        // std::cout << "-> no header present" << std::endl;
        // reset file to begin of file
        fid.clear();
        fid.seekg(0, std::ios::beg);
    }

    // set number format to read
    if (strcmp(number_format, "dec") == 0) {
        fid.setf(std::ios_base::dec, std::ios_base::basefield);
    } else if (strcmp(number_format, "hex") == 0) {
        fid.setf(std::ios_base::hex, std::ios_base::basefield);
    } else if (strcmp(number_format, "oct") == 0) {
        fid.setf(std::ios_base::oct, std::ios_base::basefield);
    } else {
        char error_msg[200];
        sprintf(error_msg, "%s%s%s", "[dec] ",
                LOCATION("Unknown number format: "), number_format);
        throw std::runtime_error(error_msg);
    }

    // read all numbers into vector
    // TODO: read rowise only, that is we have a vector for of vectors, each
    // one
    // representing one row
    std::istream_iterator<int> start(fid), end;
    std::vector<unsigned int> numbers(start, end);

    return numbers;
}

// get code from codebook member or read it from file
std::vector<unsigned int> HeuristicBaseDecoder::getSyndrome()
{
    EV_INFO << TAGGET << "syndrome<" << m_num_baseflit.top() << ","
            << m_num_codeflit.top() << "> ";

    // codebook key
    std::pair<int, int> key(m_num_baseflit.top(), m_num_codeflit.top());

    // syndrome not in codebook already
    if (m_codebook.find(key) == m_codebook.end()) {
        EV_INFO << "from file\n";
        m_codebook[key] = readSyndromeFromFile();
    } else {
        // EV << "key (" << std::get<0> (key) << "," << std::get<1> (key) <<
        // ")
        // already in map";
        EV_INFO << "from map\n";
    }

    return m_codebook[key];
}

// TODO: translate syndrome to (unsigned 32 bit) to 8-bit array
// maybe compare strings
bool HeuristicBaseDecoder::checkDecodability()
{
    EV_INFO << TAGUSE << m_codetype << " heuristic decoder\n";

    // assert(m_num_baseflit.top() <= 32); // readSyndromeFromFile should
    // throw
    // an error already

    if (m_decoding_state.top()->getWeight() < m_num_baseflit.top()) {
        EV_INFO << "need more flits\n";
        return false;
    } else {
        if (m_num_codeflit.top() == 1) {
            // special case of XORed 1 flit extension
            return true;
        } else {
            // loop through syndrome vector and check if the receive
            // sequence
            // matches any syndrome
            // a syndrome here is a receive sequence that is not decodable

            // get current codes syndrome vector
            std::vector<unsigned int> syndrome = getSyndrome();

            // use iterator to find if the received bitsequence is in the
            // syndrome vector
            std::vector<unsigned int>::iterator it;
            // for (it = syndrome.begin(); it != syndrome.end(); ++it) {
            //
            //}

            it = find(syndrome.begin(), syndrome.end(),
                      m_decoding_state.top()->getDecimalStateValue());
            // m_is_decodable = (it != syndrome.end());
            if (it != syndrome.end()) {
                // element found in vector
                EV_INFO << TAGSUCC << "syndrome match:" << *it << '\n';
                return false;
            } else {
                // element NOT found in vector
                EV_INFO << TAGFAIL << "no matching syndrome found\n";
                return true;
            }

            // loop variant
            // bool is_decodable = true;
            // for (size_t i = 0; ((is_decodable) && (i < syndrome.size()));
            // ++i) {
            //      is_decodable = !(m_received == syndrome[i]);
            // }
            // m_is_decodable = is_decodable;
        }
    }
}

/** Single heuristic decoder.
 *
 */
Define_Module(SingleHeurDecoder);
Define_Module(AggregateSingleHeurDecoder);

/** Burst heuristic decoder.
 *
 */
Define_Module(BurstHeurDecoder);
Define_Module(AggregateBurstHeurDecoder);

/** Joint heuristic decoder.
 *
 */
Define_Module(JointHeurDecoder);
Define_Module(AggregateJointHeurDecoder);
