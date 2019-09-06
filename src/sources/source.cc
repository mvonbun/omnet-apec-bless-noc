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
// File      : source.cc
// Created   : 12 Apr 2017 for apec_bless_noc
// Author(s) : Michael Vonbun
// E-mail(s) : michael.vonbun@tum.de
// 

// stl
#include <cassert> // assert
#include <cmath>
#include <cstdio> // printf
#include <cstdlib>

// project
#include <add_module_statistic.h>
#include <debug_messages.h>
#include <enum_definitions_m.h>
#include <enum_utilities.h>
#include <noc_flits.h>
#include <noc_messages_m.h>

// header
#include "source.h"

Define_Module(Source);

Source::Source()
    : m_send_interval{nullptr}, m_num_base_flit{nullptr}, m_dest_id{nullptr},
      m_id{0}, m_num_rows{0}, m_num_columns{0}, m_row{0}, m_column{0},
      m_ack_mode{0}, m_t_clk{0}, m_link_delay{0}, m_router_delay{0},
      m_timeout_guard{0}, m_head{nullptr}, m_num_messages_generated{0},
      m_num_packets_generated{0}
{
}

Source::~Source() {}

void Source::initialize()
{
    // get reference to volatile module parameters from ned
    m_send_interval = &par("send_interval");
    m_num_base_flit = &par("num_flit");
    m_dest_id = &par("dest_id");
    m_priority = &par("priority");

    // get static module parameters from ned
    // network parameters
    m_id = par("id").longValue();
    m_num_rows = par("num_rows").longValue();
    m_num_columns = par("num_columns").longValue();
    m_row = par("row").longValue();
    m_column = par("column").longValue();
    m_ack_mode = util::convertAckType(par("ack_mode").stringValue());

    // timing parameters
    m_t_clk = par("t_clk").doubleValue();
    m_link_delay = par("link_delay").longValue();
    m_router_delay = par("router_delay").longValue();
    m_timeout_guard = par("timeout_guard_cycles").longValue();
    m_max_codeflits = par("max_codeflits").longValue();

    // module behavior parameters
    m_message_en = par("message_en").boolValue();
    m_max_packet_length = par("max_packet_length").longValue();

    // register statistic signals
    // clang-format off
    m_packet_length_signal = util::addModuleStatistic(
        this, "src-", m_id, "_packet_length", "src_packet_length");
    m_packet_destination_signal = util::addModuleStatistic(
        this, "src-", m_id, "_packet_destination", "src_packet_destination");
    m_packet_debug_signal = util::addModuleStatistic(
            this, "src-", m_id, "_packet_debug", "src_packet_debug");
// clang-format on

// ini output
#ifdef MODULEINIPRINTF
    EV_INFO << TAGINI << "source[" << m_id << "]: "
            << ((m_dest_id->longValue() > -1) ? ("enabled") : ("disabled"))
            << "\n";
// EV_INFO << TAGINI << "source row: " << m_row << "\n";
// EV_INFO << TAGINI << "source column: " << m_column << "\n";
// EV_INFO << TAGINI << "signal id is: " << m_packet_length_signal << "\n";
#endif

    // start transmission
    if (m_dest_id->longValue() > -1) {
        scheduleAt(getNextGenerationTime(), new cMessage);
    }
}

void Source::finish()
{
#ifdef MODULEFINISHPRINTF
    EV_DEBUG << "[src" << m_id << "] <" << m_num_packets_generated
             << "> packets generated\n";
#endif
    recordScalar("num_packets_generated", m_num_packets_generated);
}

void Source::handleMessage(cMessage *msg)
{
// add validation code here
#ifdef VALIDATE
    std::cout << "t=" << simTime();
    printf("\t%10s%-10s\t kind=%2d\t msgid=%-8ld",
           this->getParentModule()->getFullName(), ".source", msg->getKind(),
           msg->getId());
    std::cout << std::endl;
#endif

// add debug code here
#ifdef DEBUGENABLE
    if (msg->getId() == DEBUGMSGID) {
        std::cout << this->getFullPath() << ":" << simTime() << std::endl;
        EV_DEBUG << "!!!\n";
    }
#endif

    if (msg->isSelfMessage()) {
        // Use module's parameters and random number generator
        // to make new meassage / packet.
        // Generate and send new message or packet.
        generateAndSend();

        // Trigger new event reusing the message.
        scheduleAt(getNextGenerationTime(), msg);
    } else {
        // External triggers.
        if (msg->getKind() == PACKTRIG) { // Trace replay message.
            PacketGenerationTriggerMsg *trigger =
                check_and_cast<PacketGenerationTriggerMsg *>(msg);
            sendNewHead(trigger->getNum_base_flit(), trigger->getDestination(),
                        trigger->getPriority());
            delete msg;

        } else if (util::isHead(msg)) { // Relay incoming head.
            send(msg, "out");

        } else {
            assert(false && "Invalid packet generation trigger message.");
        }
    }
}

void Source::generateAndSend(int num_base_flit, int destination, int priority)
{
    // Draw values from random number generator if default (-1) is provided.
    // clang-format off
    if (num_base_flit == -1) num_base_flit = m_num_base_flit->longValue();
    if (destination == -1) destination = m_dest_id->longValue();
    if (priority == -1) priority = m_priority->longValue();
    // clang-format on

    // generate new message or new packet
    if (m_message_en) {
        sendNewMessage(num_base_flit, destination, priority);
    } else {
        sendNewHead(num_base_flit, destination, priority);
    }
}

void Source::sendNewMessage(int num_base_flit, int destination, int priority)
{
    // divide the payload into multiple packets
    int num_packets = num_base_flit / m_max_packet_length;
    int last_packet_length = num_base_flit % m_max_packet_length;

    // generate the packets using the message counter as message id
    for (int i = 0; i < num_packets; ++i) {
        sendNewHead(m_max_packet_length, destination, priority,
                    m_num_messages_generated);
    }
    sendNewHead(last_packet_length, destination, priority,
                m_num_messages_generated);

    // increase message counter
    m_num_messages_generated++;
}

void Source::sendNewHead(int num_base_flit, int destination, int priority,
                         long message_id)
{
    // Generate new packet.
    // Set packet id, payload, priority, and generation time.
    m_head =
        new HeadFlit(m_num_packets_generated++, m_num_base_flit->longValue(),
                     m_priority->longValue(), simTime());
    m_head->setMessage_id(message_id);

    // Header kind.
    if (m_head->getNum_baseflit() > 1) {
        m_head->setKind(HEAD);
    } else {
        m_head->setKind(HEADTAIL);
    }

    // Routing information.
    m_head->setSource(m_id);
    m_head->setDestination(m_dest_id->longValue());

    // Packet retransmission.
    m_head->setTimeout(calculateTimeout(m_head->getDestination()));

    // User output.
    EV_INFO << TAGMADE << "header flit of packet<" << m_head->getPacket_id()
            << "> ";
    EV_INFO << "to <" << m_head->getDestination() << "> ";
    EV_INFO << "with <" << m_head->getNum_baseflit() << "> flits\n";

    EV_INFO << "[tx] head\n";

    // Statistics.
    emit(m_packet_length_signal, m_head->getNum_flit());
    emit(m_packet_destination_signal, m_head->getDestination());
    emit(m_packet_debug_signal,
         1000 * m_head->getPacket_id() +
             m_head->getDestination()); // lsbs of signal value is destination

    // Sending.
    send(m_head, "out");
    m_head = nullptr;
}

simtime_t Source::getNextGenerationTime()
{
    // We are using the rounded variant here, which can potentially generate two
    // headers / packets in the same cycle. Note, however, that this cycle we
    // are using for quantization is the NoC clock, that may be much higher than
    // a CPU clk
    // The NoC is slower, the CPU is faster.
    // Therefore, whithin one NoC cycle, the CPU could generate more than one
    // packet.

    simtime_t delta_t_clk(m_send_interval->doubleValue());

    // rounded variant
    //    delta_t_clk = floor( (delta_t_clk + (0.5 * m_t_clk)) / m_t_clk ) *
    //    m_t_clk;
    // ceiled variant (no zero delta_t_clk possible)
    //   delta_t_clk = ceil( delta_t_clk / m_t_clk ) * m_t_clk;

    delta_t_clk += (0.5 * m_t_clk);       // rounded variant
    double tmp = (delta_t_clk / m_t_clk); // both variants
    int delta_t_clk_cycles = floor(tmp);  // rounded variant
    // int delta_t_clk_cycles = ceil(tmp);         // ceiled variant
    delta_t_clk = delta_t_clk_cycles * m_t_clk; // both variants

    delta_t_clk += simTime();
    EV_DEBUG << ((m_num_packets_generated > 0) ? ("next") : ("starting"))
             << " transmission at t=" << delta_t_clk << "\n";

    return delta_t_clk;
}

int Source::getRow(const int index) { return index / m_num_columns; }

int Source::getColumn(const int index) { return index % m_num_columns; }

/** Timeout calculation.
 *
 * The base worst-case timeout of ACKs/NACKs is:
 *  - network access delay of flit (1 cycle)
 *  - (travel_distance+1) x (router_delay)
 *  - (travel_distance) x (link_delay)
 *  - ACK/NACK queuing delay (max_packet_length cycles)
 *  - network access delay of flit (1 cycle)
 *  - (travel_distance+1) x (router_delay)
 *  - (travel_distance) x (link_delay)
 */
simtime_t Source::calculateTimeout(const int destination_address)
{
    int distance = abs(getRow(destination_address) - m_row) +
                   abs(getColumn(destination_address) - m_column);
    m_head->setDistance(distance);

    // number of routers * router delay
    int network_delay_router_one_way = (distance + 1) * m_router_delay;

    // number of links between routers * link delay
    int network_delay_link_one_way = (distance * m_link_delay);

    // calculate base timeout; add to network delays
    // - 1 cycle EACH from/to tile to/from local links in both directions
    // - the maximum packet length (output queue buffer delay)
    // - a static compensation delay
    // - a guard interval
    return (2 * (network_delay_router_one_way + network_delay_link_one_way +
                 2 * m_ni_output_buffer_delay_cycles) +
            m_ni_delay_cycles + m_max_packet_length + m_max_codeflits +
            m_timeout_guard) *
           m_t_clk;
}
