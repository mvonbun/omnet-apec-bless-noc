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
// File      : pending_buffer.h
// Created   : 10 May 2017 for apec_bless_noc
// Author(s) : Michael Vonbun
// E-mail(s) : michael.vonbun@tum.de
// 

#ifndef __APEC_BLESS_NOC_NWIFACE_PENDING_BUFFER_H_
#define __APEC_BLESS_NOC_NWIFACE_PENDING_BUFFER_H_

// stl
#include <unordered_map> // std::unordered_map
#include <unordered_set> // std::unordered_set

// omnet
#include <omnetpp.h>

// project messages
#include <noc_flits.h>    // BaseFlit, HeadFlit, AckFlit
#include <noc_messages.h> // TimeoutMsg
#include <noc_packet.h>   // NocPacket, packet_t

using namespace omnetpp;

typedef std::unordered_map<long, NocPacket *> packet_buffer_t;
typedef std::unordered_map<long, TimeoutMsg *> timeout_buffer_t;

/** Container holding packets and timeouts that are pending.
 *
 */
class PendingBuffer
{
public:
    /** @name Constructor and Destructor.*/
    //@{
    PendingBuffer();
    virtual ~PendingBuffer();
    //@}

    /** @name Access basic buffer properties. */
    //@{
    size_t size();       ///< @return buffer size
    size_t getNumFlit(); ///< @return number of flits stored
    //@}

    /** @name Insert elements.*/
    //@{
    void insert(NocPacket *packet); ///< insert [in] packet
    //@}

    /** @name Erase elements.*/
    //@{
    /// Erasing does not delete the objects in the buffers.
    void erase(const long id); ///< erase packet by [in] id
    void erase(AckFlit *ack);  ///< erase packet by [in] ack flit
    void clear();              ///< clear buffer, i.e. erase all packets
    //@}

    /** @name Access elements.*/
    //@{

    /// @return packet by @param id packet id.
    /// Advance attempt counter if @param advance is true.
    NocPacket *getPacket(const long id, const bool advance = false);

    /// @return packet by @param msg timeout message.
    /// Advance attempt counter if @param advance is true.
    NocPacket *getPacket(TimeoutMsg *msg, const bool advance = false);

    /// Note: timeout sending is done by the invoking cSimpleModule.

    /// @return timeout by @param packet id.
    TimeoutMsg *getTimeout(const long id);

    /// @return timeout by @param msg timeout message.
    TimeoutMsg *getTimeout(TimeoutMsg *msg);

    /// @return timeout by @param ack acknowledge message.
    TimeoutMsg *getTimeout(AckFlit *ack);
    //@}

    /** Iterators. */
    //@{
    /// @return iterator to packet with @param id.
    /// end() iterator is returned if packet is not in buffer.
    packet_buffer_t::iterator getPacketIt(const long id);

    /// @return begin packet iterator.
    packet_buffer_t::iterator getPacketItBegin();

    /// @return end packet iterator.
    packet_buffer_t::iterator getPacketItEnd();

    /// @return iterator of packet at @param index.
    packet_buffer_t::iterator getPacketItAt(const std::size_t index);

    /// @return iterator to timeout with @param id.
    /// end() iterator is returned if timeout is not in buffer.
    timeout_buffer_t::iterator getTimeoutIt(const long id);

    /// @return begin timeout iterator.
    timeout_buffer_t::iterator getTimoutItBegin();

    /// @return end timeout iterator.
    timeout_buffer_t::iterator getTimeoutItEnd();

    /// @return iterator of timeout at @param index.
    timeout_buffer_t::iterator getTimoutItAt(const std::size_t index);
    //@}

    /** @name Erase and delete elements. */
    //@{

    /// Erase the object index and delete the object from the buffer.

    /// Erase and delete packet by @param id packet id
    void eraseAndDelete(const long id);

    /// Erase and delete packet by @param ack acknowledge message.
    void eraseAndDelete(AckFlit *ack);

    /// Erase and delete all packets and timeouts.
    void eraseAndDeleteAll();
    //@}

    /** @name Timeout retrieval (for canceling them in the cSimpleModule). */
    //@{
    /// @return the timeout message at @param index for cancellation.
    /// If used in a loop to get / cancel all timeouts, set @param sequential
    /// true, as it stores and increments the last iterator provided.
    cMessage *getTimeoutMessage(const std::size_t index,
                                const bool sequential = false);
    //@}

    /** @name UI output. */
    void printBufferStatus(); ///< Print buffer status to EV_INFO.

    /// Print status of packet with [in] id to EV_INFO.
    void printPacketStatus(const long id);
    //@}

    /** @name Statistics. */
    //@{
    simtime_t getAvgFillLevel(); ///< @return average buffer fill level
    //@}

protected:
    bool has(const long id); ///< check by id if packet is in pending buffer
    bool
    has(TimeoutMsg *msg); ///< check by timeout if packet is in pending buffer
    bool hasPacket(const long id);  ///< check only in packet buffer
    bool hasTimeout(const long id); ///< check only in timeout buffer

    TimeoutMsg *newTimeout(const simtime_t t_timeout, const long id);

    void clearPackets();  ///< clear packet buffer
    void clearTimeouts(); ///< clear timeout buffer

    void eraseAndDeletePacket(const long id);  ///< delete packet by id
    void eraseAndDeleteTimeout(const long id); ///< delete timeout by id
    void eraseAndDeletePackets();              ///< delete packets
    /// Delete timeouts.
    /// Note, they have to be canceled by the invoking module prior to deletion
    void eraseAndDeleteTimeouts();

    void updateAvgFillLevel(); ///< update average buffer fill level

private:
    /** @name Containers. */
    //@{
    packet_buffer_t m_packets;   ///< packet buffer
    timeout_buffer_t m_timeouts; ///< timeout buffer
    //@}

    /** @name State variables. */
    //@{
    timeout_buffer_t::iterator m_timeout_it;
    //@}

    /** @name Predefined return values. */
    //@{
    NocPacket *m_no_packet;   ///< invalid packet
    TimeoutMsg *m_no_timeout; ///< invalid timeout message
    //@}

    /** @name Statistic records. */
    //@{
    simtime_t m_t_last;     ///< timestamp of last updateAvgFillLevel call
    simtime_t m_fill_level; ///< average buffer fill level
    int m_num_requested_deleted_timeouts;
    //@}
};

//}; // namespace
#endif /* __APEC_BLESS_NOC_NWIFACE_PENDING_BUFFER_H_ */
