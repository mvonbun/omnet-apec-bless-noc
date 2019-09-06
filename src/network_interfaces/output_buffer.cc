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
// File      : output_buffer.cc
// Created   : 10 May 2017 for apec_bless_noc
// Author(s) : Michael Vonbun
// E-mail(s) : michael.vonbun@tum.de
// 

#include <enum_utilities.h> // util::isHead, util::isTail
#include <output_buffer.h>

OutputBufferBase::OutputBufferBase() {}

OutputBufferBase::~OutputBufferBase() { clear(); }

bool OutputBufferBase::push(AckFlit *ack, const bool dup)
{
    bool was_empty = empty();
    push(ack, m_ack_buffer, dup);
    return was_empty;
}

bool OutputBufferBase::duplicateAndPush(NocPacket &packet)
{
    return push(packet, true);
}

bool OutputBufferBase::push(NocPacket &packet, const bool dup)
{
    bool was_empty = empty();
    for (std::size_t i = 0; i < packet.size(); ++i) {
        push(packet.at(i), m_packet_buffer, dup);
    }
    return was_empty;
}

bool OutputBufferBase::push(cMessage *msg, const bool dup)
{
    bool was_empty = empty();
    if (msg->getKind() == ACK) {
        pushAck(msg, dup);
    } else {
        if (msg->getKind() == NACK) {
            pushNack(msg, dup);
        } else {
            pushRegularFlit(msg, dup);
        }
    }
    return was_empty;
}

bool OutputBufferBase::empty()
{
    return (packetBufferEmtpy() && ackBufferEmpty());
}

int OutputBufferBase::packetQueueSize() { return m_packet_buffer.getLength(); }

int OutputBufferBase::ackQueueSize() { return m_ack_buffer.getLength(); }

void OutputBufferBase::printStatus()
{
    EV_INFO << TAGSZO << "output buffer <" << m_packet_buffer.getLength() << ":"
            << m_ack_buffer.getLength() << ">\n";
}

void OutputBufferBase::clear()
{
    m_packet_buffer.clear();
    m_ack_buffer.clear();
}

void OutputBufferBase::pushAck(cMessage *msg, const bool dup)
{
    // Insert ack only if the ack queue does not contain an ack for that very
    // packet already. This way, overly long ack Queues are prevented.
    bool q_has_ack_already = false;
    long packet_id = check_and_cast<AckFlit *>(msg)->getPacket_id();
    for (int i = 0; i < m_ack_buffer.getLength(); ++i) {
        if (check_and_cast<AckFlit *>(m_ack_buffer.get(i))->getPacket_id() ==
            packet_id) {
            q_has_ack_already = true;
            break;
        }
    }
    if (not q_has_ack_already) {
        push(msg, m_ack_buffer, dup);
    } else {
        delete msg;
    }
}

void OutputBufferBase::pushNack(cMessage *msg, const bool dup)
{
    // Insert ack only if the ack queue does not contain an ack for that very
    // packet already. This way, overly long ack Queues are prevented.
    bool q_has_ack_already = false;
    long packet_id = check_and_cast<NackFlit *>(msg)->getPacket_id();
    for (int i = 0; i < m_ack_buffer.getLength(); ++i) {
        if (check_and_cast<NackFlit *>(m_ack_buffer.get(i))->getPacket_id() ==
            packet_id) {
            q_has_ack_already = true;
            break;
        }
    }
    if (not q_has_ack_already) {
        push(msg, m_ack_buffer, dup);
    } else {
        delete msg;
    }
}

void OutputBufferBase::pushRegularFlit(cMessage *msg, const bool dup)
{
    push(msg, m_packet_buffer, dup);
}

void OutputBufferBase::push(cMessage *msg, cQueue &buffer, const bool dup)
{
    if (dup) {
        EV_DEBUG << TAGPUSH << "duplicate of msg<" << msg->getId()
                 << "> to output buffer\n";
        buffer.insert(msg->dup());
    } else {
        EV_DEBUG << TAGPUSH << "msg<" << msg->getId() << "> to output buffer\n";
        buffer.insert(msg);
    }
}

cMessage *OutputBufferBase::popPacketQueue()
{
    cMessage *msg = pop(m_packet_buffer);
    if (util::isHead(msg)) {

        EV_INFO << TAGPOP << PACKET(((HeadFlit *)msg))
                << " from output buffer\n";
    } else {
        EV_INFO << TAGPOP << "flit<" << ((RootFlit *)msg)->getSequence_number()
                << "> from output buffer\n";
    }

    return msg;
}

cMessage *OutputBufferBase::popAckQueue()
{
    EV_INFO << TAGPOP << "ack from output buffer\n";
    return pop(m_ack_buffer);
}

cMessage *OutputBufferBase::pop(cQueue &buffer)
{
    return (cMessage *)buffer.pop();
}

bool OutputBufferBase::packetBufferEmtpy() { return m_packet_buffer.isEmpty(); }

bool OutputBufferBase::ackBufferEmpty() { return m_ack_buffer.isEmpty(); }

cMessage *OutputBufferBlocking::pop(bool *last)
{
    if ((!ackBufferEmpty()) && (!m_packet_in_transmission)) {
        EV_DEBUG << TAGPOP << "from ack buffer\n";
        return OutputBufferBase::pop(m_ack_buffer);
    } else if (!packetBufferEmtpy()) {
        EV_DEBUG << TAGPOP << "from packet buffer\n";
        cMessage *msg = OutputBufferBase::pop(m_packet_buffer);
        if (util::isHead(msg)) {
            m_packet_in_transmission = true;
        }
        if (util::isTail(msg, true)) {
            m_packet_in_transmission = false;
            *last = true;
        }
        return msg;
    } else {
        return nullptr;
    }
}

cMessage *OutputBufferNonblockingPreserving::pop(bool *last)
{
    if (!ackBufferEmpty()) {
        return OutputBufferBase::pop(m_ack_buffer);
    } else if (!packetBufferEmtpy()) {
        return OutputBufferBase::pop(m_packet_buffer);
    } else {
        return nullptr;
    }
}

cMessage *OutputBufferNonblockingNonpreserving::pop(bool *last)
{
    if (!ackBufferEmpty()) {
        EV_DEBUG << TAGPOP << "from ack buffer\n";
        if (m_packet_in_transmission) {
            cMessage *msg = OutputBufferBase::pop(m_packet_buffer);
            if (util::isTail(msg, true)) {
                m_packet_in_transmission = false;
                *last = true;
            }
            delete msg;
        }
        return OutputBufferBase::pop(m_ack_buffer);
    } else if (!packetBufferEmtpy()) {
        EV_DEBUG << TAGPOP << "from packet buffer\n";
        cMessage *msg = OutputBufferBase::pop(m_packet_buffer);
        if (util::isHead(msg)) {
            m_packet_in_transmission = true;
        }
        if (util::isTail(msg, true)) {
            m_packet_in_transmission = false;
            *last = true;
        }
        return msg;
    } else {
        return nullptr;
    }
}

// void enqueue(std::vector<BaseFlit *> packet); //< enqueue packet; return
//                                              // true if buffer was empty
// void enqueue(cMessage *flit); //< enqueue (ack) flit; return true if buffer
// was empty
// bool isEnqueued(const int id); //< check if packet is in queue
// void dequeue(const int id);    //< remove packet from queue
