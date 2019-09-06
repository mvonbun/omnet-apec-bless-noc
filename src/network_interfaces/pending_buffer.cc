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
// File      : pending_buffer.cc
// Created   : 10 May 2017 for apec_bless_noc
// Author(s) : Michael Vonbun
// E-mail(s) : michael.vonbun@tum.de
// 

// stl
#include <cassert>
#include <iterator>

// project
#include <debug_messages.h>

// class
#include <pending_buffer.h>

using namespace omnetpp;

PendingBuffer::PendingBuffer()
    : m_packets(), m_timeouts(), m_timeout_it{nullptr}, m_no_packet{nullptr},
      m_no_timeout{nullptr}, m_t_last{0}, m_fill_level{0},
      m_num_requested_deleted_timeouts{0}
{
}

PendingBuffer::~PendingBuffer()
{
#ifdef MSGDESTRUCTORPRINTF
    EV_DEBUG << "<" << m_num_requested_deleted_timeouts
             << "> timeouts requested that have been deleted before\n";
    eraseAndDeleteAll();
#endif
}

void PendingBuffer::insert(NocPacket *packet)
{
    if (has(packet->id())) {
        EV_DEBUG << "tried to insert packet with id <" << packet->id() << ">.";
        EV_DEBUG << "This packet is already in the buffer.";
    }
    assert(!has(packet->id()));
    EV_DEBUG << TAGPUSH << PACKET(packet->head()) << "to pending buffer\n";
    m_packets[packet->id()] = packet;
    m_timeouts[packet->id()] =
        newTimeout((packet->head())->getTimeout(), packet->id());
}

void PendingBuffer::erase(const long id)
{
    m_packets.erase(id);
    m_timeouts.erase(id);
}

void PendingBuffer::erase(AckFlit *ack) { erase(ack->getPacket_id()); }

void PendingBuffer::clear()
{
    clearPackets();
    clearTimeouts();
}

NocPacket *PendingBuffer::getPacket(const long id, const bool advance)
{
    if (hasPacket(id)) {
        if (advance) {
            (m_packets[id]->head())->newAttempt();
        }
        return m_packets[id];
    } else {
        return m_no_packet; // return nullptr;
    }
}

NocPacket *PendingBuffer::getPacket(TimeoutMsg *msg, const bool advance)
{
    return getPacket(msg->getPacket_id(), advance);
}

TimeoutMsg *PendingBuffer::getTimeout(const long id)
{
    if (hasTimeout(id)) {
        // udpate the timeout everytime if the head has multiple timeouts
        // (i.e. it is some kind of repetition code on a packet basis)
        if (m_packets[id]->head()->hasMutlipleTimeouts()) {
            m_timeouts[id]->setTimeout(m_packets[id]->head()->getTimeout());
        }
        return m_timeouts[id];
    } else {
        ++m_num_requested_deleted_timeouts;
        return m_no_timeout;
    }
}

TimeoutMsg *PendingBuffer::getTimeout(TimeoutMsg *msg)
{
    return getTimeout(msg->getPacket_id());
}

TimeoutMsg *PendingBuffer::getTimeout(AckFlit *ack)
{
    return getTimeout(ack->getPacket_id());
}

void PendingBuffer::eraseAndDelete(const long id)
{
    eraseAndDeletePacket(id);
    eraseAndDeleteTimeout(id);
}

void PendingBuffer::eraseAndDelete(AckFlit *ack)
{
    eraseAndDelete(ack->getPacket_id());
}

void PendingBuffer::eraseAndDeleteAll()
{
    eraseAndDeletePackets();
    eraseAndDeleteTimeouts();
}

size_t PendingBuffer::size() { return m_packets.size(); }

size_t PendingBuffer::getNumFlit()
{
    size_t num_flit_stored = 0;
    for (packet_buffer_t::iterator it = m_packets.begin();
         it != m_packets.end(); ++it) {
        num_flit_stored += (it->second)->size();
    }
    return num_flit_stored;
}

void PendingBuffer::printBufferStatus()
{
    EV_INFO << "<" << size() << "> packets pending\n";
}

void PendingBuffer::printPacketStatus(const long id)
{
    if (hasPacket(id)) {
        EV_INFO << "packet at <" << id << "> is <" << (m_packets[id]->info())
                << "\n";
    } else {
        EV_INFO << "there is no packet pending with <" << id << ">\n";
    }
}

simtime_t PendingBuffer::getAvgFillLevel()
{
    updateAvgFillLevel();
    return (m_fill_level / simTime());
}

bool PendingBuffer::has(const long id)
{
    return ((hasPacket(id)) && hasTimeout(id));
}

bool PendingBuffer::has(TimeoutMsg *msg) { return (has(msg->getPacket_id())); }

bool PendingBuffer::hasPacket(const long id)
{
    return (m_packets.find(id) != m_packets.end());
}

bool PendingBuffer::hasTimeout(const long id)
{
    return (m_timeouts.find(id) != m_timeouts.end());
}

TimeoutMsg *PendingBuffer::newTimeout(const simtime_t t_timeout, const long id)
{
    // TimeoutMsg* msg = new TimeoutMsg;
    // msg->setPacket_id(id);
    // msg->setKind(TIMEOUT);
    // msg->setSchedulingPriority(SP_LATE);
    // msg->setTimeout(t_timeout);
    // return msg;
    return new TimeoutMsg(id, t_timeout, SP_LATE, TIMEOUT);
}

void PendingBuffer::eraseAndDeletePacket(const long id)
{
    if (hasPacket(id)) {
        delete m_packets[id];             // delete packet (i.e. map value) //
                                          // m_packets[id]->erase();
        assert(m_packets.erase(id) == 1); // delete id (i.e. map key)
    }
}

void PendingBuffer::eraseAndDeleteTimeout(const long id)
{
    if (hasTimeout(id)) {
        // TODO: check if the class is able to delete;
        // otherwise, return pointer to erased timeout and call delete by the
        // invoking module
        delete m_timeouts[id];             //  and delete event (i.e. map value)
        assert(m_timeouts.erase(id) == 1); // delete id (i.e. map key)
    }
}

void PendingBuffer::eraseAndDeletePackets()
{
    for (packet_buffer_t::iterator it = m_packets.begin();
         it != m_packets.end(); ++it) {
        // ((*it).second)->erase();
        delete it->second;
    }
    m_packets.clear();
}

void PendingBuffer::clearPackets() { m_packets.clear(); }

void PendingBuffer::clearTimeouts() { m_timeouts.clear(); }

void PendingBuffer::eraseAndDeleteTimeouts()
{
    for (timeout_buffer_t::iterator it = m_timeouts.begin();
         it != m_timeouts.end(); ++it) {
        delete it->second;
    }
    m_timeouts.clear();
}

void PendingBuffer::updateAvgFillLevel()
{
    m_fill_level = m_fill_level + (simTime() - m_t_last) * m_packets.size();
    m_t_last = simTime();
}

cMessage *PendingBuffer::getTimeoutMessage(const std::size_t index,
                                           const bool sequential)
{
    // get iterator to message at index
    if (index < m_timeouts.size()) {
        if (sequential) {
            if (index == 0) {
                m_timeout_it = m_timeouts.begin();
            } else
                ++m_timeout_it;
        } else {
            m_timeout_it = m_timeouts.begin();
            if (index > 0) {
                std::advance(m_timeout_it, index);
            }
        }

        return m_timeout_it->second; // return message
    } else {
        return nullptr;
    }
}

packet_buffer_t::iterator PendingBuffer::getPacketIt(const long id)
{
    return m_packets.find(id);
}

packet_buffer_t::iterator PendingBuffer::getPacketItBegin()
{
    return m_packets.begin();
}

packet_buffer_t::iterator PendingBuffer::getPacketItEnd()
{
    return m_packets.end();
}

packet_buffer_t::iterator PendingBuffer::getPacketItAt(const std::size_t index)
{
    packet_buffer_t::iterator it = m_packets.begin();
    if (index > 0) {
        std::advance(it, index);
    }
    return it;
}

timeout_buffer_t::iterator PendingBuffer::getTimeoutIt(const long id)
{
    return m_timeouts.find(id);
}

timeout_buffer_t::iterator PendingBuffer::getTimoutItBegin()
{
    return m_timeouts.begin();
}

timeout_buffer_t::iterator PendingBuffer::getTimeoutItEnd()
{
    return m_timeouts.end();
}

timeout_buffer_t::iterator PendingBuffer::getTimoutItAt(const std::size_t index)
{
    timeout_buffer_t::iterator it = m_timeouts.begin();
    if (index > 0) {
        std::advance(it, index);
    }
    return it;
}
