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
// File      : output_buffer.h
// Created   : 10 May 2017 for apec_bless_noc
// Author(s) : Michael Vonbun
// E-mail(s) : michael.vonbun@tum.de
// 

#ifndef __APEC_BLESS_NOC_NWIFACE_OUTPUT_BUFFER_H_
#define __APEC_BLESS_NOC_NWIFACE_OUTPUT_BUFFER_H_

// omnet
#include <omnetpp.h>

// project utilities
#include <debug_messages.h>     // TAGs, PACKET and FLIT macro
#include <enum_definitions_m.h> // scheduling priorities, message types
#include <enum_utilities.h>     // util::isHead, util::isTail

// project messages
#include <noc_flits.h>    // BaseFlit, HeadFlit, AckFlit
#include <noc_messages.h> // TimeoutMsg
#include <noc_packet.h>   // NocPacket

// module

using namespace omnetpp;

/** Output queue wrapper baser class.
 *
 * Uses two queues to store ACKs and regular flits separately.
 *
 */
class OutputBufferBase
{
public:
    OutputBufferBase();
    virtual ~OutputBufferBase();

    /** @name Push interface. */
    /// Push messages or packets to the buffer.
    /// Return true if the buffer was empty before pushing.
    //@{

    /// Push @param ack flit. Duplicate packet if @param dup is true.
    bool push(AckFlit *ack, const bool dup = false);

    /// Push entire noc @param packet. Duplicate packet if @param dup is true.
    bool push(NocPacket &packet, const bool dup = false);

    /// Push entire noc packet by duplicating.
    bool duplicateAndPush(NocPacket &packet);

    /// Push single @param msg. Duplicate the message if @param dup is true.
    bool push(cMessage *msg, const bool dup = false);
    //@}

    /** @name Pop interface. */
    //@{
    /// @return front message; if buffer is empty, return nullptr instead.
    virtual cMessage *pop(bool *last) = 0;
    //@}

    /** @name Status methods. */
    //@{
    bool empty();          ///< @return true if buffer is empty.
    int packetQueueSize(); ///< @return the number of flits in the packet queue
    int ackQueueSize();    ///< @return the number of acks in the ack queue
    void printStatus();    ///< Print buffer status to EV_INFO.
    void clear();          ///< Clears the buffer (deletes the messages).
    //@}

protected:
    /** @name Low level method implementations. */
    //@{
    void pushAck(cMessage *msg, const bool dup = false);
    void pushNack(cMessage *msg, const bool dup = false);
    void pushRegularFlit(cMessage *msg, const bool dup = false);
    void push(cMessage *msg, cQueue &buffer, const bool dup = false);

    cMessage *popPacketQueue();
    cMessage *popAckQueue();
    cMessage *pop(cQueue &buffer);

    bool packetBufferEmtpy();
    bool ackBufferEmpty();
    //@}

    /** @name Buffer. */
    //@{
    cQueue m_packet_buffer; ///< Regular flits / packet buffer.
    cQueue m_ack_buffer;    ///< Ack buffer.
    //@}
    // std::unordered_set<int> m_enqueued_packets; // manage ids of enqueued
    // packets
};

/** Blocking Output Buffer.
 *
 * Prevents packets from being split by acks.
 *
 */
class OutputBufferBlocking : public OutputBufferBase
{
public:
    OutputBufferBlocking() : m_packet_in_transmission(false){};
    virtual cMessage *pop(bool *last) override;

private:
    /// Transmission state; true if a packet is being sent, false otherwise.
    bool m_packet_in_transmission;
};

/** Nonblocking Preserving Output Buffer.
 *
 * Sends ACKs asap but preserves all flits, i.e. if a packet is interleaved by
 * an ACK, the flits following the ACK are effectively delayed by one cycle.
 *
 */
class OutputBufferNonblockingPreserving : public OutputBufferBase
{
public:
    virtual cMessage *pop(bool *last) override;
};

/** Nonblocking Nonpreserving Output Buffer.
 *
 * Sends ACKs asap. If a packet flit is in transmission, the ACK supersedes the
 * packet flit.
 */
class OutputBufferNonblockingNonpreserving : public OutputBufferBase
{
public:
    OutputBufferNonblockingNonpreserving() : m_packet_in_transmission(false){};
    virtual cMessage *pop(bool *last) override;

private:
    /// Transmission state; true if a packet is being sent, false otherwise.
    bool m_packet_in_transmission;
};

#endif /* __APEC_BLESS_NOC_NWIFACE_OUTPUT_BUFFER_H_ */
