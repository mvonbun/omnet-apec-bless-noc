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
// File      : ctrl.cc
// Created   : 05 Apr 2019 for apec_bless_noc
// Author(s) : Michael Vonbun
// E-mail(s) : michael.vonbun@tum.de
// 

#include "ctrl.h"
#include "enum_definitions_m.h"
#include "noc_messages.h"
#include <add_module_statistic.h>

Define_Module(Ctrl);

Ctrl::Ctrl()
    : m_num_nodes{0}, m_conv_th{0}, m_module_has_converged{false},
      m_conv_value_last{0}, m_start_time{0}, m_simulation_has_converged{false},
      m_simulation_ended_overflow{false}
{
}

Ctrl::~Ctrl() {}

void Ctrl::initialize()
{
    // module parameters
    m_num_nodes = par("num_nodes").longValue();
    m_conv_th = par("converge_threshold").doubleValue();

    // module members
    m_module_has_converged.resize(m_num_nodes, false);
    m_conv_value_last.resize(m_num_nodes, 0);

    // start time
    time(&m_start_time);
}

void Ctrl::finish()
{
    // record simulation times
    recordScalar("SimTimeEnd", simTime());

    time_t current_time;
    time(&current_time);
    recordScalar("CTimeStart", m_start_time);
    recordScalar("CTimeEnd", current_time);
    recordScalar("TimeElapsed", difftime(current_time, m_start_time));

    // if convergence check was enabled and all tiles have converged at the end
    // of the simulation, record the convergence threshold. Otherwise, record a
    // 0 indicating no convergence.
    if (m_simulation_has_converged) {
        recordScalar("Convergence", m_conv_th);
    } else {
        recordScalar("Convergence", 0);
    }

    // record exit reason
    if (m_simulation_ended_overflow) {
        recordScalar("ExitState", -1);
    } else if (m_simulation_has_converged) {
        recordScalar("ExitState", 1);
    } else {
        recordScalar("ExitState", 0);
    }

    // write the simulation times to std::out
    // (we use EV_WARN to have it also in Cmdenv mode)
    struct tm *timeinfo;
    timeinfo = localtime(&current_time);
    EV_WARN << "CTRL: Simulation ended on " << asctime(timeinfo);
    EV_WARN << "CTRL: Simulation needed "
            << difftime(current_time, m_start_time) << " seconds\n";
}

void Ctrl::handleMessage(cMessage *msg)
{
    if (msg->getKind() == CCONV) {
        ConvergenceMsg *conv_msg = check_and_cast<ConvergenceMsg *>(msg);

        // calculate absolute difference
        simtime_t abs_diff;
        if (conv_msg->getAvg_latency() >
            m_conv_value_last[conv_msg->getModule_id()]) {
            abs_diff = conv_msg->getAvg_latency() -
                       m_conv_value_last[conv_msg->getModule_id()];
        } else {
            abs_diff = m_conv_value_last[conv_msg->getModule_id()] -
                       conv_msg->getAvg_latency();
        }

        // compare against threshold; end simulation if all modules converged
        if (abs_diff < m_conv_th) {
            EV_DEBUG << "Sink " << conv_msg->getModule_id() << " converged\n";
            m_module_has_converged[conv_msg->getModule_id()] = true;

            if (have_all_converged()) {
                m_simulation_has_converged = true;
                delete msg;
                EV_WARN << "! Simulation converged at t = " << simTime()
                        << "\n";
                endSimulation();
            }

            // update comparison register if current module did not converge
        } else {
            m_conv_value_last[conv_msg->getModule_id()] =
                conv_msg->getAvg_latency();
        }

    } else if (msg->getKind() == CBUFOVFL) {
        m_simulation_ended_overflow = true;
        PBuffOverflowMsg *overflow = check_and_cast<PBuffOverflowMsg *>(msg);
        recordScalar("BufferOverflowID", overflow->getModule_id());
        delete msg;
        endSimulation();

    } else {
        EV_DEBUG << "CTRL: unknown message type received\n";
    }

    delete msg;
}

bool Ctrl::have_all_converged()
{
    // loop over stored convergence states
    bool all_converged = true;
    for (size_t i = 0; i < m_module_has_converged.size(); i++) {
        all_converged &= m_module_has_converged[i];
    }
    return all_converged;
}
