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
// File      : noc_packet.cc
// Created   : 12 May 2017 for apec_bless_noc
// Author(s) : Michael Vonbun
// E-mail(s) : michael.vonbun@tum.de
// 

#include <noc_packet.h>

#include <cassert>

#include <enum_definitions_m.h>
#include <enum_utilities.h>

NocPacket::NocPacket()
    : m_head(), m_packet(), m_is_initialized{false}, m_it{nullptr}
{
}

NocPacket::NocPacket(HeadFlit *head)
    : m_head(head), m_packet(head->getNum_flit()), m_is_initialized{true},
      m_it(m_packet.begin())
{
}

NocPacket::~NocPacket() { eraseAndDeleteAll(); }

long NocPacket::id() { return (empty()) ? (-1) : (m_head->getPacket_id()); }

size_t NocPacket::size() { return m_packet.size(); }

bool NocPacket::empty() { return m_packet.empty(); }

std::string NocPacket::info()
{
    return (std::to_string(m_head->getPacket_id()) + ":" +
            std::to_string(m_packet.size()) + ":" +
            std::to_string(m_head->getSource()) + "->" +
            std::to_string(m_head->getDestination()) + ":" +
            std::to_string(m_head->getAttempts()));
}

void NocPacket::push_back(cMessage *msg)
{
    if (util::isHead(msg)) {
        push_back(check_and_cast<HeadFlit *>(msg));
    } else {
        push_back(check_and_cast<RootFlit *>(msg));
    }
}

void NocPacket::push_back(HeadFlit *head)
{
    if (m_is_initialized) {
        m_packet.insert(m_it++, head);
    } else {
        assert(empty()); // insert heads only in empty packet containers
        m_head = head;
        m_packet.push_back(head);
    }
}

void NocPacket::push_back(RootFlit *flit)
{
    if (m_is_initialized) {
        m_packet.insert(m_it++, flit);
    } else {
        m_packet.push_back(flit);
    }
}

HeadFlit *NocPacket::head() { return m_head; }

RootFlit *NocPacket::at(const unsigned i) { return m_packet.at(i); }

void NocPacket::erase(const unsigned i)
{
    assert(i < m_packet.size()); // prevent out-of-bounds indices
    const packet_base_t::iterator it = (m_packet.begin() + i);
    erase(it);
}

packet_base_t::iterator NocPacket::erase(const packet_base_t::iterator it)
{
    return m_packet.erase(it);
}

packet_base_t::iterator NocPacket::erase(const packet_base_t::iterator first,
                                         const packet_base_t::iterator last)
{
    return m_packet.erase(first, last);
}

void NocPacket::clear() { m_packet.clear(); }

void NocPacket::eraseAndDelete(const unsigned i)
{
    assert(i < m_packet.size()); // prevent out-of-bounds indices
    const packet_base_t::iterator it = (m_packet.begin() + i);
    eraseAndDelete(it);
}

packet_base_t::iterator
NocPacket::eraseAndDelete(const packet_base_t::iterator it)
{
    delete *it;
    return m_packet.erase(it);
}

packet_base_t::iterator
NocPacket::eraseAndDelete(const packet_base_t::iterator first,
                          const packet_base_t::iterator last)
{
    for (packet_base_t::iterator it = first; it != last; ++it) {
        delete *it;
    }
    return m_packet.erase(first, last);
}

void NocPacket::eraseAndDeleteAll()
{
    for (packet_base_t::iterator it = m_packet.begin(); it != m_packet.end();
         ++it) {
        delete *it;
    }
    m_packet.clear();
}
