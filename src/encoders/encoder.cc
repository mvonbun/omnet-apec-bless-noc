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
// File      : encoder.cc
// Created   : 21 Mar 2017 for apec_bless_noc
// Author(s) : Michael Vonbun
// E-mail(s) : michael.vonbun@tum.de
// 

// stl
#include <cassert> // assert
#include <cstdio>  // printf
#include <stdexcept>
#include <string>

// project
#include <debug_messages.h>
#include <enum_definitions_m.h>
#include <enum_utilities.h>
#include <heuristic_code.h>
#include <noc_flits.h>

// header
#include "encoder.h"

// pure virtual base class; therefore, no module definition
// Define_Module(Encoder);
Encoder::Encoder()
    : m_head{nullptr}, m_flit{nullptr}, m_t_clk{0}, m_num_baseflit{0},
      m_num_codeflit{0}, m_encoding_length_threshold{0},
      m_router_release_mode{0}, m_blocksize{0}
{
}

Encoder::~Encoder() { delete m_flit; }

void Encoder::initialize()
{
    // get static module parameters from ned
    m_t_clk = par("t_clk").doubleValue();
    m_num_codeflit = par("num_codeflit").longValue();
    m_encoding_length_threshold = par("length_threshold").longValue();
    m_router_release_mode = util::convertResourceReleaseType(
        par("router_release_mode").stringValue());
    m_blocksize = par("blocksize").longValue();
}

void Encoder::finish() {}

/** Handle incoming messages.
 *
 * The only messages arriving at this module are headers
 * from the source.
 *
 */
void Encoder::handleMessage(cMessage *msg)
{
// add validation code here
#ifdef VALIDATE
    if (!msg->isSelfMessage()) {
        std::cout << "t=" << simTime();
        printf("\t%10s%-10s\t kind=%2d\t msgid=%-8ld",
               this->getParentModule()->getFullName(), ".encoder",
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

    if (util::isHead(msg)) {
        // cast the incoming message and store for further usage
        m_head = check_and_cast<HeadFlit *>(msg);
        EV_INFO << "[rx] head\n";

        // get and set current head properties
        getHeadProperties(m_head);
        setHeadProperties(m_head);

        // encode only if packet length is above threshold
        if (m_num_baseflit > m_encoding_length_threshold) {
            // 1-flit packets are handled separately
            if (m_num_baseflit == 1) {
                sendRepeatedHeads(m_num_codeflit);
            } else {
                EV_INFO << TAGENC << PACKET(m_head) << " using ";
                encode();
            }

        } else { // do not encode
            EV_INFO << "Length of " << PACKET(m_head) << " is below length "
                    << "threshold<" << m_encoding_length_threshold << ">\n";
            EV_INFO << "   => no redundancy added to this packet\n";
            m_head->setNum_codeflit(0);
            sendSequence(m_num_baseflit);
        }
    } else {
        assert(false && "ENC: received non-head flit");
    }
}

/// extract header properties
void Encoder::getHeadProperties(HeadFlit *head)
{
    m_num_baseflit = head->getNum_baseflit();
}

/// set encoder specific header properties
void Encoder::setHeadProperties(HeadFlit *head)
{
    //    head->setCode_type(m_code_type);
    head->setNum_codeflit(m_num_codeflit);
}

/** Send enumerated sequence of flits.
 *
 * sendSequence(\c num_flit, \c last_flit_tail)
 *  sends an enumerated sequence of \c num_flit flits
 *  starting with the header.
 *  If \c last_flit_tail is \c TRUE,
 *  the sequence is terminated with a tail flit.
 *  Otherwise it is terminated by a regular body flit.
 *
 */
void Encoder::sendSequence(int num_flit, bool last_flit_tail)
{
    // forward header
    //  sequence number 0 (set in message definition)
    send(m_head, "out");
    if (num_flit > 1) {
        if (num_flit > 2) {
            // send body flits
            //  header and terminating flit count towards num_flit as well
            sendFlits(num_flit - 2, 1, false);
        }
        // send terminating flit (tail or body )
        //  body sequence numbers go from 1..(num_flit - 2)
        //  so last flit is num_flit -2
        sendFlits(1, num_flit - 1, last_flit_tail);
    }
}

// make and send NUM_FLIT payload flits with sequence number starting at offset
void Encoder::sendFlits(int num_flit, int offset, bool is_tail)
{
    // BaseFlit *flit;
    for (int i = 0; i < num_flit; ++i) {
        //        flit = generateFlit(offset + i, is_tail);
        //        send(flit, "out");
        generateFlit(offset + i, is_tail);
        send(m_flit, "out");
    }
    m_flit = nullptr; // invalidate flit after sending
}

// generate new flit
// BaseFlit* Encoder::generateFlit(int sequence_num, bool is_tail) {
//    BaseFlit *flit = new BaseFlit();
//    flit->setIs_head(false);
//    flit->setIs_tail(is_tail);
//    flit->setSequence_number(sequence_num);
//    (is_tail) ? (flit->setKind(TAIL)) : (flit->setKind(BODY));
//    return flit;
//}

void Encoder::generateFlit(int sequence_num, bool is_tail)
{
    if (is_tail) {
        m_flit = new TailFlit(sequence_num, TAIL);
    } else {
        m_flit = new BodyFlit(sequence_num, BODY);
    }
}

void Encoder::sendRepeatedHeads(const int num_heads)
{
    // m_head->setNum_codeflit(0);
    // forward header copies
    //    for (int i = 0; i < num_heads; ++i) {
    // send(m_head->dup(), "out"); // duplicate message and send the copy
    m_head->addRepetitionInterval(0);
    //    }
    send(m_head, "out"); // forward header at the end
}

//
// derived encoders
//

// uncoded encoder
Define_Module(UncodedEncoder);
void UncodedEncoder::encode()
{
    EV_INFO << "UncodedEncoder\n";
    sendSequence(m_num_baseflit);
}

// cyclic encoder
Define_Module(CyclicEncoder);
void CyclicEncoder::encode()
{
    EV_INFO << "CyclicEncoder\n";
    // send base packet
    // do not set tail if number of code flits positive
    sendSequence(m_num_baseflit, !(m_num_codeflit > 0));

    // send cyclic repetition
    int k = 0;
    for (int i = 0; i < m_num_codeflit; ++i) {
        // get sequence number
        if (i % (m_num_baseflit - 1) == 0) {
            k = 0;
        }

        if (i < (m_num_codeflit - 1)) {
            sendFlits(1, ++k, false); // send body flit
        } else {
            sendFlits(1, ++k, true); // send tail flit
        }
    }
}

// simple encoder (pure virtual)
void SimpleEncoder::encode(const char *encoder_name)
{
    EV_INFO << encoder_name << "\n";
    sendSequence(m_num_baseflit + m_num_codeflit);
}

void SimpleEncoder::setHeadProperties(HeadFlit *head)
{
    // adjust ACK/NACK timeout
    head->addTimeout((m_num_baseflit + m_num_codeflit) * m_t_clk);

    // set common head properties
    Encoder::setHeadProperties(head);
}

// dynamic encoder
Define_Module(DynamicEncoder);
void DynamicEncoder::encode() { SimpleEncoder::encode("DynamicEncoder"); }

// fec encoder
Define_Module(FecEncoder);
void FecEncoder::encode() { SimpleEncoder::encode("FecEncoder"); }

// single heuristic encoder
Define_Module(SingleHeurEncoder);
void SingleHeurEncoder::encode() { SimpleEncoder::encode("SingleHeurEncoder"); }

// burst heuristic encoder
Define_Module(BurstHeurEncoder);
void BurstHeurEncoder::encode() { SimpleEncoder::encode("BurstHeurEncoder"); }

// joint heuristic encoder
Define_Module(JointHeurEncoder);
void JointHeurEncoder::encode() { SimpleEncoder::encode("JointHeurEncoder"); }

// interleaved block xor encoder
Define_Module(InterleavedBlockXEncoder);
void InterleavedBlockXEncoder::setHeadProperties(HeadFlit *head)
{
    // encode only the payload flits
    // m_num_baseflit := packet length (incl. header and (possibly) tail)
    // num_codeflit = ceil(num_baseflit - 1 / blocksize)
    int num_payload_flit_to_encode;
    if (m_router_release_mode == TAILFLAG) {
        num_payload_flit_to_encode = m_num_baseflit - 2;
    } else {
        num_payload_flit_to_encode = m_num_baseflit - 1;
    }

    // calculate number of code flits
    m_num_codeflit = num_payload_flit_to_encode / m_blocksize +
                     (num_payload_flit_to_encode % m_blocksize != 0);

    // save to head
    SimpleEncoder::setHeadProperties(head);
}

void InterleavedBlockXEncoder::encode()
{
    SimpleEncoder::encode("InterleavedBlockXEncoder");
}

// void Encoder::encode() {
// EV << "Encoder\n";
//    EV << "Encoding packet: " << convertCodeType(m_code_type) << "(";
//    EV << m_head->getNum_baseflit() << ","
//              << m_head->getNum_baseflit() + m_head->getNum_codeflit() <<
//              ")\n";
//
//    switch (m_code_type) {
//    case UNCODED:
//        uncoded();
//        break;
//    case CYCLIC:
//        cyclic();
//        break;
//    case DYNAMIC:
//        dynamic();
//        break;
//    case HEUR_S:
//        heur_s();
//        break;
//    case HEUR_B:
//        heur_b();
//        break;
//    case HEUR_J:
//        heur_j();
//        break;
//    case FEC:
//        fec();
//        break;
//    default:
//        std::invalid_argument("Encoder::encode::Code Type");
//        break;
//    }
//
//}

//
//
//
// make and send cyclic extended packet
// void Encoder::generateAndSend_cyclicPacket() {
//    int num_total_flit = m_num_flit + m_code_strength - 1; // total flits to
//    send (header was sent already)
//
//    int k = 0;
//    for (int i = 0; i < num_total_flit; ++i) {
//        // get sequence number
//        if (i % (m_num_flit - 1) == 0) {
//            k = 0;
//        }
//
//        if (i < num_total_flit - 1) {
//            // send body flit
//            sendFlits(1, ++k, false);
//        } else {
//            // send tail flit
//            sendFlits(1, ++k, true);
//        }
//    }
//}
