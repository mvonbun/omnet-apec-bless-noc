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
// File      : decoder.h
// Created   : 21 Mar 2017 for apec_bless_noc
// Author(s) : Michael Vonbun
// E-mail(s) : michael.vonbun@tum.de
// 

#ifndef __APEC_BLESS_NOC_DECODER_H_
#define __APEC_BLESS_NOC_DECODER_H_

// stl
#include <cstdint>       // typedefs
#include <fstream>       //
#include <map>           // std::map
#include <stack>         // std::stack
#include <string>        // std::string
#include <unordered_map> // std::unordered_map
#include <unordered_set> // std::unordered_set
#include <utility>       // pair, get<> pair
#include <vector>        // std::vector

// omnet
#include <omnetpp.h>
using namespace omnetpp;

// project utilities

// project messages
class RootFlit;
class HeadFlit;

// module
class BaseDecoderHousekeeper;
class CodedDecoderHousekeeper;
class DecodingState;

/** Pure virtual base decoder class.
 *
 *  Used to validate packets, generate ACKs and forward headers.
 *
 *  Basic operation is divided into three stages:
 *  (1) Incoming messages are checked for their type and handled
 *      accordingly
 *  (2) Flits are decoded using a two step process
 *  (3) On success, headers are forwarded to the sink and acks are
 *      generated for the network
 *
 *  The two step decoding process is basically:
 *  (a) use function decode(flit) to registers the sequence number of
 *      the incoming flit in member m_received
 *  (b) use function checkDecodability() to use the information stored
 *      in the members to check if the received sequence suffices to
 *      decode the packet
 *
 *
 *  For a reference on the usage of the codes, have a look at the
 *  src/codes folder and its testbenches in sandbox/.
 *
 *  The implementation of the decoding scheme is done by the derived
 *  classes.
 *
 */
class Decoder : public cSimpleModule
{
public:
    Decoder();
    virtual ~Decoder();

protected:
    /* * * * * Omnet++ Interface. * * * * */
    virtual void initialize() override;
    virtual void finish() override;
    virtual void handleMessage(cMessage *msg) override;

    virtual void childInitialize() = 0;

    /* * * * * Members. * * * * */
    /** @name Module parameters. */
    //@{
    int m_id;                  ///< module id
    simtime_t m_t_clk;         ///< clk to model decoding time
    long m_num_nodes;          ///< number of nodes in the network
    int m_router_release_mode; ///< Resource release mode of routers.
    int m_ack_mode;            ///< ACK or NACK acknowledgments.
    bool m_aggregate_en;       ///< Use packet aggregation.
    //@}

    //    bool m_router_flag_release_mode; ///< Resource release mode of
    //    routers.

    // self message
    cMessage *m_tail_event;
    cMessage *m_instant_tail_event;

    /** @name Data members. */
    /// As it might occur that 2 (or more) packets arrive interleaved,
    /// also the decoder has to account for this.
    /// Therefore, we use stacks instead of scalar variables, to be able to
    /// start new decodings and resume from previous decoding, also if no tail
    /// was received in the meantime.
    //@{
    RootFlit *m_flit;               ///< Current flit buffer.
    bool m_flit_is_head;            ///< Current flit type.
    bool m_flit_is_tail;            ///< Current flit type.
    std::stack<HeadFlit *> m_head;  ///< Header stack.
    std::stack<int> m_num_flit;     ///< Packet length stack.
    std::stack<int> m_num_baseflit; ///< Data length stack.
    std::stack<int> m_num_codeflit; ///< Redundancy length stack.
    //@}

    /** @name Status members. */
    //@{
    std::stack<bool> m_is_enabled;   ///< Decoder enable.
    std::stack<bool> m_is_decodable; ///< Success indicator.
    std::stack<bool> m_was_received; ///< Was current packet received earlier?
    std::stack<bool> m_failure;      ///< Did decoding fail?

    /// Index of assumed next packet, one per other network node.
    BaseDecoderHousekeeper *m_housekeeper;
    virtual BaseDecoderHousekeeper *newHousekeeper(const int num_nodes) = 0;
    //@}

    /** @name Decoder delays in cycles after decoding success. */
    //@{
    std::stack<int> m_head_forward_delay;
    int m_ack_delay;
    //@}

    /* * * * * Methods. * * * * */
    /** @name Base class interface. */
    /// Decoder functions implemented by the derived classes.
    //@{
    virtual void processNewHead();
    virtual void setCodeDelays() = 0;
    virtual void decode(RootFlit *flit) = 0;
    virtual bool checkDecodability() = 0;
    virtual void popStack();

    virtual void actionOnSuccess();
    virtual void actionOnFailure();

    virtual void recordStatistic() = 0;
    //@}

    /** @name Incoming message handling. */
    //@{
    void handleFlit(cMessage *msg); ///< entry point: handle flits
    void handleHead(cMessage *msg); ///< handle packet head: start new decode
    void handleEnd();               ///< handle tail flits: finish decoding
    //@}

    /** @name Main interface. */
    //@{
    void process(); ///< process flits
    //@}

    /** @name Utilities. */
    //@{
    void getHeadProperties(); ///< extract header properties
    void deleteFlit();        ///< conditionally delete flits
    void forwardHead();
    void sendAck();
    void sendAck(int ack_delay);
    void sendNack();
    //@}

    /** @name Signals. */
    //@{
    simsignal_t m_signal_num_flit;
    simsignal_t m_signal_num_flit_rx_before;
//@}

/// Debug
#ifdef DEBUGENABLE
    std::ofstream m_file;
    int m_debug_enable; // id of decoder that is debugged
#endif
};

/** Uncoded decoder.
 *
 *  Used only if packet integrity is ensured by the parameters.
 */
class UncodedDecoder : public Decoder
{
public:
    virtual ~UncodedDecoder();

protected:
    virtual void childInitialize() override;

    virtual BaseDecoderHousekeeper *
    newHousekeeper(const int num_nodes) override;

    virtual void setCodeDelays() override;
    virtual void decode(RootFlit *flit) override;
    virtual bool checkDecodability() override;
    virtual void recordStatistic() override;
    virtual void popStack() override;

    BaseDecoderHousekeeper *m_housekeeper_impl;

    int m_ack_delay_policy;
    int m_latency_policy;
};

/** Base simple decoder.
 *  The simple decoder merely counts the number of received flits.
 */
class SimpleDecoder : public Decoder
{
public:
    SimpleDecoder();
    virtual ~SimpleDecoder();

protected:
    /** @name Interface. */
    //@{
    virtual void childInitialize() override;

    virtual BaseDecoderHousekeeper *
    newHousekeeper(const int num_nodes) override;

    virtual void processNewHead() override;
    virtual void setCodeDelays() override;
    virtual void decode(RootFlit *flit) override;
    virtual bool checkDecodability() override;
    virtual void recordStatistic() override;
    virtual void popStack() override;
    //@}

    /** @name Main data members. */
    //@{
    std::stack<DecodingState *> m_decoding_state; ///< Decoding status stack.
    //@}

    /** @name Decoder flavor members. */
    //@{
    std::string m_codetype;
    CodedDecoderHousekeeper *m_housekeeper_impl;
    //@}

    /** @name Signals. */
    //@{
    simsignal_t m_signal_num_flit_received; /// Head-to-tail flits.
    simsignal_t m_signal_num_flit_needed;   /// Payload flits.
    simsignal_t m_signal_nondecodable_num_flit;
    simsignal_t m_signal_num_flit_missing; /// Flits missing to decode.
    //@}
};

/** Aggregate simple decoder.
 *
 *  Resumes the decoding process where previous attempts stopped.
 *
 *  Extends the simple decoder by loading the decoding result of
 *  previous attempts first.
 *
 * 	Therefore, a packet could be received by aggregating /
 * 	collecting packet junks over time.
 */
class AggregateSimpleDecoder : public virtual SimpleDecoder
{
public:
    virtual ~AggregateSimpleDecoder();

protected:
    virtual BaseDecoderHousekeeper *
    newHousekeeper(const int num_nodes) override;

    //    virtual void actionOnFailure() override;
    AggregateCodedDecoderHousekeeper *m_ext_housekeeper;
};

/** Cyclic decoder.
 *	Cyclic extension, uses the sequence numbers to determine success.
 */
class CyclicDecoder : public SimpleDecoder
{
public:
    CyclicDecoder() { m_codetype = "cyclic"; };
};
class AggregateCyclicDecoder : public AggregateSimpleDecoder
{
public:
    AggregateCyclicDecoder() { m_codetype = "cyclic"; };
};

/** Interleaved Block-XOR decoder.
 *  Assumes that the payload is split in blocks of length N.
 *  It checks if in a block of N+1 consecutive flits at least N flits are
 * received.
 *  Only if all blocks are decodable the packet is decodable as well.
 *
 */
class InterleavedBlockXDecoder : public SimpleDecoder
{
public:
    InterleavedBlockXDecoder()
    {
        m_codetype = "interblockxor";
        m_blocksize = 0;
    };

protected:
    virtual void childInitialize() override;
    virtual bool checkDecodability() override;
    bool isBlockDecodable(int block_index, bool is_last_block);
    int getNumberOfElementsOfBlock(bool is_last_block);
    int m_blocksize;
};
/** Interleaved Block-XOR decoder, aggregated.
 *  Same as base interleaved block-wise XOR, but allows to resume from a
 * previous decoding state.
 */
class AggregateInterleavedBlockXDecoder : public InterleavedBlockXDecoder
{
public:
    virtual ~AggregateInterleavedBlockXDecoder();

protected:
    virtual BaseDecoderHousekeeper *
    newHousekeeper(const int num_nodes) override;
    AggregateCodedDecoderHousekeeper *m_ext_housekeeper;
};

/** Dynamic decoder.
 *  Named version of the simple decoder.
 */
class DynamicDecoder : public SimpleDecoder
{
public:
    DynamicDecoder() { m_codetype = "dynamic"; };
};
class AggregateDynamicDecoder : public AggregateSimpleDecoder
{
public:
    AggregateDynamicDecoder() { m_codetype = "dynamic"; };
};

/** FEC decoders.
 *  Named version of the simple decoder.
 */
class FecDecoder : public SimpleDecoder
{
public:
    FecDecoder() { m_codetype = "fec"; };
};
class AggregateFecDecoder : public AggregateSimpleDecoder
{
public:
    AggregateFecDecoder() { m_codetype = "fec"; };
};

/** Heuristic base decoder.
 *  Base class for the heuristic decoders.
 */
class HeuristicBaseDecoder : public virtual SimpleDecoder
{
private:
    // m_codebook is a map holding the syndromes for all codes requested
    //    so far, so one does not have to read them all new from the
    //    syndrome file every time
    std::map<std::pair<int, int>, std::vector<unsigned int>> m_codebook;
    std::vector<unsigned int>
    readSyndromeFromFile(const char *number_format = "dec",
                         const char *comment_start = "#");

protected:
    std::vector<unsigned int> getSyndrome();
    virtual bool checkDecodability();
};

/** Aggregate version of the base heuristic decoder.
 *
 *
 */
class AggregateHeuristicBaseDecoder : public AggregateSimpleDecoder,
                                      HeuristicBaseDecoder
{
protected:
    virtual void actionOnSuccess()
    {
        AggregateSimpleDecoder::actionOnSuccess();
    };
    virtual void actionOnFailure()
    {
        AggregateSimpleDecoder::actionOnFailure();
    };
};

/** Single heuristic decoder.
 *  Decoder designed to compensate independent erasures.
 */
class SingleHeurDecoder : public HeuristicBaseDecoder
{
public:
    SingleHeurDecoder() { m_codetype = "single"; };
};
class AggregateSingleHeurDecoder : public AggregateHeuristicBaseDecoder
{
public:
    AggregateSingleHeurDecoder() { m_codetype = "single"; };
};

/** Burst heuristic decoder.
 *  Decoder designed to compensate bursty erasures.
 */
class BurstHeurDecoder : public HeuristicBaseDecoder
{
public:
    BurstHeurDecoder() { m_codetype = "burst"; };
};
class AggregateBurstHeurDecoder : public AggregateHeuristicBaseDecoder
{
public:
    AggregateBurstHeurDecoder() { m_codetype = "burst"; };
};

/** Joint heuristic decoder.
 *  Decoder designed to compensate both bursty and independent erasures.
 */
class JointHeurDecoder : public HeuristicBaseDecoder
{
public:
    JointHeurDecoder() { m_codetype = "joint"; };
};
class AggregateJointHeurDecoder : public AggregateHeuristicBaseDecoder
{
public:
    AggregateJointHeurDecoder() { m_codetype = "joint"; };
};

#endif
