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
// File      : source.h
// Created   : 12 Apr 2017 for apec_bless_noc
// Author(s) : Michael Vonbun
// E-mail(s) : michael.vonbun@tum.de
// 

#ifndef __APEC_BLESS_NOC_SOURCE_H_
#define __APEC_BLESS_NOC_SOURCE_H_

// omnet
#include <omnetpp.h>
using namespace omnetpp;

// project messages
class HeadFlit;

/** Simple Source.
 *
 *  Generates headers and sends them to the encoder.
 *
 *  The source is responsible for setting the timeout based on the network
 * properties
 *  - network access delay
 *  - router and link delays
 *  - network size
 *  - maximum packet payload
 */
class Source : public cSimpleModule
{
public:
    Source();
    virtual ~Source();

protected:
    virtual void initialize() override;
    virtual void finish() override;
    virtual void handleMessage(cMessage *msg) override;

private:
    /** @name Volatile parameters. Implemented as references. */
    //@{
    cPar *m_send_interval;
    cPar *m_num_base_flit;
    cPar *m_dest_id;
    cPar *m_priority;
    //@}

    /** @name Static network parameters. */
    //@{
    int m_id;          ///< Module id.
    int m_num_rows;    ///< Number of rows (2D-mesh NoC).
    int m_num_columns; ///< Number of columns (2D-mesh NoC).
    int m_row;         ///< Module row.
    int m_column;      ///< Module column.
    int m_ack_mode;    ///< Which acknowledgment to use.
    //@}

    /** @name Static timing parameters. */
    //@{
    simtime_t m_t_clk;   ///< Clock period.
    int m_link_delay;    ///< Delay of links in cycles.
    int m_router_delay;  ///< Delay of routers in cycles.
    int m_timeout_guard; ///< Timeout guard in cycles.
    //@}

    /** @name Static module parameters. */
    //@{
    /// Control message sending by restricting the packet length.
    /// If enabled, messages are split into smaller packets of length
    /// m_max_packet_length.
    bool m_message_en; ///< Enable message mechanism.
    /// Used to split messages and to have the timeout account for the queuing
    /// delay of ACKs/NACKs of blocking network interface.
    int m_max_packet_length;
    /// Maximum number of codeflits added (used to determine the real max.
    /// packet length).
    int m_max_codeflits;
    //@}

    /** @name Statistics signals. */
    //@{
    simsignal_t m_packet_length_signal;
    simsignal_t m_packet_destination_signal;
    simsignal_t m_packet_debug_signal;
    //@}

    /** @name Internal state registers. */
    //@{
    HeadFlit *m_head;              ///< Temporary header buffer.
    long m_num_messages_generated; ///< Total number of messages generated.
    long m_num_packets_generated;  ///< Total number of packets/heads generated.

    /// Network Interface Delay Compensation in cycles.
    /// Although the decoder generates the ACK as soon as possible
    /// (leading to an ACK being generated in the same cycle as the packet
    /// arrived at the decoder) the network interface needs at least one cycle
    /// to insert and pop the ACK from the output buffer.
    /// This is close to real world behavior, as it is unlikely that a packets
    /// gets acked in the very same cycle.
    ///
    /// Set to 0 to cause frequent 'send packet before ack arrived'.
    const int m_ni_delay_cycles = 1;
    const int m_ni_output_buffer_delay_cycles = 1;
    //@}

    /** @name Class methods. */
    //@{
    /// Main message / packet sending method.
    /// Use module random number generators if parameter default (-1) is used.
    void generateAndSend(int num_base_flit = -1, int destination = -1,
                         int priority = -1);

    void sendNewMessage(int num_base_flit, int destination, int priority);

    void sendNewHead(int num_base_flit, int destination, int priority,
                     long message_id = -1);

    /// @return row of module with @param index.
    int getRow(const int index);

    /// @return column of module with @param index.
    int getColumn(const int index);

    /// @return timeout for a packet to @param destination_address.
    simtime_t calculateTimeout(const int destination_address);

    /// @return clock-quantized simulation time of next header generation
    simtime_t getNextGenerationTime();
    //@}
};

#endif
