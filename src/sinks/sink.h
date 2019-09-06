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
// File      : sink.h
// Created   : 08 Apr 2017 for apec_bless_noc
// Author(s) : Michael Vonbun
// E-mail(s) : michael.vonbun@tum.de
// 

#ifndef __APEC_BLESS_NOC_SINK_H_
#define __APEC_BLESS_NOC_SINK_H_

// omnet
#include <omnetpp.h>
using namespace omnetpp;

// project messages
class HeadFlit;

/** Sink class.
 *
 *
 */
class Sink : public cSimpleModule
{
public:
    Sink() : m_num_packets_received(0){};
    ~Sink();

private:
    /** @name Module parameters. */
    //@{
    simtime_t m_t_clk;               ///< Clock period.
    int m_id;                        ///< module id.
    simtime_t m_conv_check_interval; ///< Interval of convergence checking.
    //@}

    /** @name Module members. */
    //@{
    HeadFlit *m_head;            ///< packet head
    long m_num_packets_received; ///< state counter
    simtime_t m_conv_acc_t;      ///< accumulator of convergence latency
    simtime_t m_conv_t_check;    ///< time of last convergence check
    //@}

    /** @name Signals. */
    //@{
    simsignal_t m_latency_generation;
    simsignal_t m_latency_injection;
    simsignal_t m_latency_network_access;
    simsignal_t m_attempts;
    //@}

protected:
    virtual void initialize() override;
    virtual void finish() override;
    virtual void handleMessage(cMessage *msg) override;

    void statisticsCollection(HeadFlit *head);
    void statisticsCollectionEmit(const simtime_t delay_generation,
                                  const simtime_t delay_injection,
                                  const simtime_t delay_network_access,
                                  const int attempts);
    void statisticsCollectionEmit(const int delay_generation,
                                  const int delay_injection,
                                  const int delay_network_access,
                                  const int attempts);
    void sendToNetworkController(cMessage *msg);
};

#endif
