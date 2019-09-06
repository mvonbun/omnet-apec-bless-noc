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
// File      : noc_packet.h
// Created   : 12 May 2017 for apec_bless_noc
// Author(s) : Michael Vonbun
// E-mail(s) : michael.vonbun@tum.de
// 

#ifndef NOC_MESSAGES_NOC_PACKET_H_
#define NOC_MESSAGES_NOC_PACKET_H_

#include <string> // std::string
#include <vector> // std::vector

#include <omnetpp.h>

#include <noc_flits.h> // message definitions

using namespace omnetpp;

typedef std::vector<RootFlit *> packet_base_t;

/// A packet container for holding the flits of a packet.
///
/// Basically wraps around std::vector.
///
class NocPacket
{
public:
    /** @name Constructor and Destructor. */
    //@{
    NocPacket();
    NocPacket(HeadFlit *head);
    virtual ~NocPacket();
    //@}

    /** @name Access basic packet properties. */
    //@{
    long id();     ///< @return non-negative packet id
    size_t size(); ///< @return size of packet
    bool empty();  ///< @return true check if packet is empty
    //@}

    /** @name UI output. */
    //@{
    /// Return formatted packet information.
    /// Format: id:len:src->dest:attempts
    std::string info();
    //@}

    /** @name Insert messages. */
    //@{
    void push_back(cMessage *msg); ///< [in] message to add.
    /// [in] header flit. Fails if packet is not empty.
    void push_back(HeadFlit *head);
    void push_back(RootFlit *flit); ///< [in] regular flit.
    //@}

    /** @name Access elements. */
    //@{
    HeadFlit *head(); ///< @return pointer to packet header
    RootFlit *at(const unsigned
                     i); ///< @return pointer to flit in packet by [in] position
    //@}

    /** @name Erase elements. */
    //@{
    /// Erasing does not not destroy the objects in the packet.
    void erase(const unsigned i); ///< erase element at [in] index

    /// @return valid iterator after erasing element at
    /// @param it position passed by iterator
    packet_base_t::iterator erase(const packet_base_t::iterator it);

    /// @return valid iterator after erasing elements from
    /// @param first start position to
    /// @param last finish position.
    packet_base_t::iterator erase(const packet_base_t::iterator first,
                                  const packet_base_t::iterator last);

    void clear(); ///< clear entire packet (i.e. erase all)
    //@}

    /** @name Erase and delete elements. */
    //@{
    /// The objects are deleted and the index is erased.
    void eraseAndDelete(const unsigned i); ///< remove element by [in] index

    /// @return valid iterator after erasing and deletring element at
    /// @param it position passed by iterator
    packet_base_t::iterator eraseAndDelete(const packet_base_t::iterator it);

    /// @return valid iterator after erasing and deleting elements from
    /// @param first start position to
    /// @param last finish position.
    packet_base_t::iterator eraseAndDelete(const packet_base_t::iterator first,
                                           const packet_base_t::iterator last);
    /// erase and delete all elements; similar to clear but destroys the
    /// contained objects
    void eraseAndDeleteAll();
    //@}

private:
    HeadFlit *m_head;            ///< reference to packet header
    packet_base_t m_packet;      ///< packet
    const bool m_is_initialized; ///< flag to indicate that packet has been
                                 /// empty initialized
    packet_base_t::iterator m_it;
};

/// typedef NocPacket packet_t; // deprecated

#endif /* NOC_MESSAGES_NOC_PACKET_H_ */
