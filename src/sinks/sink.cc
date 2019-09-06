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
// File      : sink.cc
// Created   : 08 Apr 2017 for apec_bless_noc
// Author(s) : Michael Vonbun
// E-mail(s) : michael.vonbun@tum.de
// 

// stl
#include <cassert> // assert
#include <cstdio>  // printf

// project
#include <add_module_statistic.h>
#include <debug_messages.h>
#include <enum_definitions_m.h>
#include <enum_utilities.h>
#include <noc_flits.h>
#include <noc_messages.h>

// header
#include "sink.h"

Define_Module(Sink);

Sink::~Sink()
{
    EV_DEBUG << "<" << m_num_packets_received << "> packets/headers received\n";
}

void Sink::initialize()
{
    // get static module parameters from ned
    m_id = par("id").longValue();
    m_t_clk = par("t_clk").doubleValue();
    m_conv_t_check = par("convergence_check_interval").doubleValue();

    // register statistic signals
    // clang-format off
    m_latency_generation = util::addModuleStatistic(
        this, "sink-", m_id, "_latency_generation_cycles", "sink_latency_generation_cycles");
    m_latency_injection = util::addModuleStatistic(
        this, "sink-", m_id, "_latency_injection_cycles", "sink_latency_injection_cycles");
    m_latency_network_access = util::addModuleStatistic(
        this, "sink-", m_id, "_latency_network_access_cycles", "sink_latency_network_access_cycles");
    m_attempts = util::addModuleStatistic(
        this, "sink-", m_id, "_attempts", "sink_attempts");
    // clang-format on
}

void Sink::finish()
{
#ifdef MODULEFINISHPRINTF
    EV_DEBUG << "[sink" << m_id << "] <" << m_num_packets_received
             << "> packets received\n";
#endif
    recordScalar("num_packets_received", m_num_packets_received);
}

// TODO
void Sink::handleMessage(cMessage *msg)
{
// add validation code here
#ifdef VALIDATE
    if (!msg->isSelfMessage()) {
        std::cout << "t=" << simTime();
        printf("\t%10s%-10s\t kind=%2d\t msgid=%-8ld",
               this->getParentModule()->getFullName(), ".sink", msg->getKind(),
               msg->getId());
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

    if (msg->arrivedOn("abort_simulation_port")) {
        // for some reason, num_cycles_max lead to negative cycles in the
        // signals recording
        // so we just use INT_MAX as maximum number of cycles instead
        // int num_cycles_max = (int)(SIMTIME_MAX / m_t_clk);
        statisticsCollectionEmit(INT_MAX, INT_MAX, INT_MAX, INT_MAX);
        EV_WARN << "! Pending buffer overflow at module " << m_id
                << " at t = " << simTime() << "\n";
        sendToNetworkController(msg);

    } else {
        // message arrived on input port
        if (util::isHead(msg)) {
            ++m_num_packets_received;

            m_head = check_and_cast<HeadFlit *>(msg);
            EV_INFO << TAGRX << PACKET(m_head) << "\n";
            EV_INFO << "head(" << m_head->getSequence_number() << ")\n";
            //        verbose("Received head", " #" << head->getId() );
            //        verbose("Gen Time", " was " <<
            //        head->getGeneration_time());

            statisticsCollection(m_head);

        } else if (msg->getKind() == BODY) {
            EV_INFO << "body("
                    << check_and_cast<RootFlit *>(msg)->getSequence_number()
                    << ")\n";

        } else if (msg->getKind() == TAIL) {
            // no special handling of single-flit packets
            EV_INFO << " tail("
                    << check_and_cast<RootFlit *>(msg)->getSequence_number()
                    << ")\n";
        }

        delete msg;
    }
}

/// Get packet statistics.
/// The latency in cycles is the pure network travel time plus the length of
/// the packet in the case of no collisions etc.
void Sink::statisticsCollection(HeadFlit *head)
{
    // get delay / latency times
    simtime_t delay_generation_time =
        (head->getArrivalTime() - head->getCreationTime());
    simtime_t delay_injection_time =
        (head->getArrivalTime() - head->getInjection_time());
    simtime_t delay_network_access_time =
        (head->getInjection_time() - head->getCreationTime());

    // calculate delay / latency cycles
    int delay_generation_cycles = (int)(delay_generation_time / m_t_clk);
    int delay_injection_cycles = (int)(delay_injection_time / m_t_clk);
    int delay_network_access_cycles =
        (int)(delay_network_access_time / m_t_clk);

    assert(head->getCreationTime() == head->getGeneration_time());

    // console / debug output
    EV_DEBUG << "The packet was\n";
    EV_DEBUG << "   created at t=" << head->getCreationTime() << "\n";
    EV_DEBUG << "   injected at t=" << head->getInjection_time() << "\n";
    EV_DEBUG << "   received at t=" << head->getArrivalTime() << "\n";
    EV_DEBUG << "The packet needed\n";
    EV_DEBUG << "   " << delay_generation_cycles << " cycles from generation\n";
    EV_DEBUG << "   " << delay_injection_cycles << " cycles from injection\n";

    // record the delays / latencies
    statisticsCollectionEmit(delay_generation_cycles, delay_injection_cycles,
                             delay_network_access_cycles, head->getAttempts());

    if (m_conv_t_check > 0) {
        // accumulate latency for convergence check
        m_conv_acc_t += delay_generation_time;

        // check for convergence (it's done ROUGHLY every check_interval, since
        // it also depends if a packet arrived at all)
        simtime_t cur_time = simTime();
        if ((cur_time - m_conv_t_check) > m_conv_check_interval) {
            m_conv_t_check = cur_time;
            sendToNetworkController(new ConvergenceMsg(
                m_id, m_conv_acc_t / m_num_packets_received));
        }
    }
}

void Sink::statisticsCollectionEmit(const simtime_t delay_generation,
                                    const simtime_t delay_injection,
                                    const simtime_t delay_network_access,
                                    const int attempts)
{
    statisticsCollectionEmit((int)(delay_generation / m_t_clk),
                             (int)(delay_injection / m_t_clk),
                             (int)(delay_network_access / m_t_clk), attempts);
}

void Sink::statisticsCollectionEmit(const int delay_generation,
                                    const int delay_injection,
                                    const int delay_network_access,
                                    const int attempts)
{
    emit(m_latency_generation, delay_generation);
    emit(m_latency_injection, delay_injection);
    emit(m_latency_network_access, delay_network_access);
    emit(m_attempts, attempts);
}

void Sink::sendToNetworkController(cMessage *msg)
{
    cModule *targetModule =
        getParentModule()->getParentModule()->getSubmodule("ctrl");
    sendDirect(msg, targetModule, "port");
}
