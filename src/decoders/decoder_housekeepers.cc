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
// File      : decoder_housekeepers.cc
// Created   : 22 May 2017 for apec_bless_noc
// Author(s) : Michael Vonbun
// E-mail(s) : michael.vonbun@tum.de
// 

#include "decoder_housekeepers.h"

/** Successfully Received Packet Housekeeper.
 *
 */
BaseDecoderHousekeeper::BaseDecoderHousekeeper(const int num_nodes)
    : m_next_packet_ids(num_nodes, 0),
      m_tmp_id_buffer(num_nodes, std::unordered_set<long>())
{
}

BaseDecoderHousekeeper::~BaseDecoderHousekeeper() {}

void BaseDecoderHousekeeper::updateReceived(const int source, const long id)
{
    long &next_packet_id = m_next_packet_ids[source];

    if (id == next_packet_id) {
        // current packet is next expected one => increment next expected one
        EV_DEBUG << "current packet<" << id << "::" << source
                 << "> is next expected one\n";
        ++next_packet_id;

        // as the main list got updated, check if ids in tmp buffer continue the
        // sequence of ids <=> search for consecutive ids in tmp buffer
        if (!(m_tmp_id_buffer[source]).empty()) {
            long next_id = (id + 1);
            if (isInTmpIdBuffer(source, next_id)) {
                // move id from tmp buffer to main list
                EV_DEBUG << "move already received packet index <" << next_id
                         << "::" << source << "> from buffer to list\n";
                m_tmp_id_buffer[source].erase(next_id); // erase from map
                updateReceived(source, next_id); // write as next_packet_id
            }
        }
    } else {
        m_tmp_id_buffer[source].insert(id); // put the index in temp buffer
    }
}

void BaseDecoderHousekeeper::updateReceived(HeadFlit *head)
{
    updateReceived(head->getSource(), head->getPacket_id());
}

bool BaseDecoderHousekeeper::wasReceived(const int source, const long id)
{
    return ((id < m_next_packet_ids[source]) || isInTmpIdBuffer(source, id));
}

bool BaseDecoderHousekeeper::wasReceived(HeadFlit *head)
{
    return wasReceived(head->getSource(), head->getPacket_id());
}

bool BaseDecoderHousekeeper::isInTmpIdBuffer(const int source, const long id)
{

    return (m_tmp_id_buffer[source].find(id) != m_tmp_id_buffer[source].end());
}

/** Coded Housekeeper that also maintains the decoding state.
 *
 */
CodedDecoderHousekeeper::CodedDecoderHousekeeper(const int num_nodes,
                                                 const bool reset_on_get)
    : BaseDecoderHousekeeper(num_nodes), m_reset_on_get(reset_on_get),
      m_decoding_state(num_nodes, std::unordered_map<long, DecodingState *>())
{
}

CodedDecoderHousekeeper::~CodedDecoderHousekeeper()
{
    std::unordered_map<long, DecodingState *>::iterator it;
    for (size_t i = 0; i < m_decoding_state.size(); ++i) {
        for (it = m_decoding_state[i].begin(); it != m_decoding_state[i].end();
             ++it) {
            delete it->second;
        }
        m_decoding_state[i].clear();
    }
}

DecodingState *CodedDecoderHousekeeper::getState(const int source,
                                                 const long id,
                                                 const int num_flit)
{
    // if the packet wasn't received successfully already and is not already in
    // the decoding state buffer, create new decoding state
    if ((!wasReceived(source, id)) && (!hasPacket(source, id))) {
        m_decoding_state[source][id] = new DecodingState(num_flit);
    }

    // housekeeper flavor, i.e. aggregated or non-aggregated controlled here
    if (m_reset_on_get) {
        m_decoding_state[source][id]->reset();
    }

    return m_decoding_state[source][id];
}

DecodingState *CodedDecoderHousekeeper::getState(HeadFlit *head)
{
    return getState(head->getSource(), head->getPacket_id(),
                    head->getNum_flit());
}

void CodedDecoderHousekeeper::updateReceived(const int source, const long id)
{
    BaseDecoderHousekeeper::updateReceived(source, id);
    // erase successfully decoded packet state from state buffer
    if (hasPacket(source, id)) {
        delete m_decoding_state[source][id];
        m_decoding_state[source].erase(id);
    }
}

bool CodedDecoderHousekeeper::hasPacket(const int source, const long id)
{
    return (m_decoding_state[source].find(id) !=
            m_decoding_state[source].end());
}

bool CodedDecoderHousekeeper::hasPacket(HeadFlit *head)
{
    return (hasPacket(head->getSource(), head->getPacket_id()));
}

/** Partially Received Packet Housekeeper.
 *
 */
AggregateCodedDecoderHousekeeper::AggregateCodedDecoderHousekeeper(
    const int num_nodes)
    : CodedDecoderHousekeeper(num_nodes, false)
{
}

AggregateCodedDecoderHousekeeper::~AggregateCodedDecoderHousekeeper() {}

// DecodingState *AggregateCodedDecoderHousekeeper::getState(const int source,
//                                                           const int id,
//                                                           const int num_flit)
// {
//     if (hasPacket(source, id)) {
//         return m_decoding_state[source][id]; // return saved state
//     } else {
//         return new DecodingState(num_flit); // return fresh
//     }
// }

// DecodingState *AggregateCodedDecoderHousekeeper::getState(HeadFlit *head)
// {
//     return getState(head->getSource(), head->getPacket_id(),
//                     head->getNum_baseflit());
// }

// void AggregateCodedDecoderHousekeeper::updateReceived(const int source,
//                                                       const int id)
// {
//     // update already received manager
//     BaseDecoderHousekeeper::updateReceived(source, id);

//     // erase successfully decoded packet from partially received ones
//     if (hasPacket(source, id)) {
//         delete m_decoding_state[source][id]; // TODO: check where to delete
//         m_decoding_state[source].erase(id);
//     }
// }

// void AggregateCodedDecoderHousekeeper::updateReceived(HeadFlit *head)
// {
//     updateReceived(head->getSource(), head->getPacket_id());
// }

// bool AggregateCodedDecoderHousekeeper::hasPacket(const int source, const int
// id)
// {
//     return (m_decoding_state[source].find(id) !=
//             m_decoding_state[source].end());
// }

// bool AggregateCodedDecoderHousekeeper::hasPacket(HeadFlit *head)
// {
//     return (hasPacket(head->getSource(), head->getPacket_id()));
// }

// void AggregateCodedDecoderHousekeeper::saveState(const int source, const int
// id,
//                                                  DecodingState *state)
// {
//     m_decoding_state[source][id] = state;
// }

// void AggregateCodedDecoderHousekeeper::saveState(HeadFlit *head,
//                                                  DecodingState *state)
// {
//     saveState(head->getSource(), head->getPacket_id(), state);
// }
