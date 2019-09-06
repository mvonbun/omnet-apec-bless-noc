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
// File      : decoder_housekeepers.
// Created   : 22 May 2017 for apec_bless_noc
// Author(s) : Michael Vonbun
// E-mail(s) : michael.vonbun@tum.de
// 

#ifndef DECODERS_DECODER_HOUSEKEEPERS_H_
#define DECODERS_DECODER_HOUSEKEEPERS_H_

// stl
#include <unordered_map> // std::unordered_map
#include <unordered_set> // std::unordered_set
#include <vector>        // std::vector

// omnet
#include <omnetpp.h>

// project utilities

// project messages
#include "noc_flits.h"

// module
#include "decoding_state.h"

using namespace omnetpp;

/** Housekeeper that keeps track of already successfully received packets.
 *
 *  wasReceived
 *  Used to check if a packet has been successfully received already.
 *
 *  updateReceived
 *  Used to update the housekeeper with a successfully received packet.
 *
 */
class BaseDecoderHousekeeper
{
public:
    BaseDecoderHousekeeper(const int num_nodes);
    virtual ~BaseDecoderHousekeeper();

    /** @name Update housekeeper with new successful packet. */
    //@{
    virtual void updateReceived(const int source, const long id);
    virtual void updateReceived(HeadFlit *head);
    //@}

    /** @name Current packet checking. */
    //@{
    bool wasReceived(const int source, const long id);
    bool wasReceived(HeadFlit *head);
    //@}

private:
    /// Low level function to check if packet is in tmp id buffer.
    bool isInTmpIdBuffer(const int source, const long id);

    /// Next packet id, one per network node.
    std::vector<long> m_next_packet_ids;

    /// Temporary id buffer, one per network node.
    /// This buffer is required as packets might arrive out-of-order.
    /// We use it together with the next packet id to keep housekeeping to a
    /// minimum. Basically, next_packet_id is updated when the entire sequence
    /// from 0 until this packet id is received. Successfully received packets
    /// that do not complete the sequence are temporarily stored in this
    /// buffer. Once the entire sequence is received, the element is erased
    /// from the temp buffer.
    std::vector<std::unordered_set<long>> m_tmp_id_buffer;
};

/** Housekeeper that also keeps track of the decoding state.
 *
 */
class CodedDecoderHousekeeper : public BaseDecoderHousekeeper
{
public:
    CodedDecoderHousekeeper(const int num_nodes,
                            const bool reset_on_get = true);
    virtual ~CodedDecoderHousekeeper();

    /** @name Get decoding state. */
    //@{
    virtual DecodingState *getState(const int source, const long id,
                                    const int num_flit);
    virtual DecodingState *getState(HeadFlit *head);
    //@}

    /** @name Update housekeeper with new successful packet. */
    //@{
    virtual void updateReceived(const int source, const long id) override;
    //@}

    /** @name Check if packet is decoding state is stored. */
    //@{
    bool hasPacket(const int source, const long id);
    bool hasPacket(HeadFlit *head);
    //@}

private:
    /// Behavior of getState.
    bool m_reset_on_get;

    /// Decoding states. One map -- mapping packet ids to states -- per network
    /// node.
    std::vector<std::unordered_map<long, DecodingState *>> m_decoding_state;
};

/** Housekeeper that keeps track of partially received packets and their decode
 * state.
 *
 *
 */
class AggregateCodedDecoderHousekeeper : public CodedDecoderHousekeeper
{
public:
    AggregateCodedDecoderHousekeeper(const int num_nodes);
    virtual ~AggregateCodedDecoderHousekeeper();
};

#endif /* DECODERS_DECODER_HOUSEKEEPERS_H_ */
