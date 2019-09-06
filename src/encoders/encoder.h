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
// File      : encoder.h
// Created   : 21 Mar 2017 for apec_bless_noc
// Author(s) : Michael Vonbun
// E-mail(s) : michael.vonbun@tum.de
// 

#ifndef __APEC_BLESS_NOC_ENCODER_H_
#define __APEC_BLESS_NOC_ENCODER_H_

// omnet
#include <omnetpp.h>
using namespace omnetpp;

// forward decleration
class HeadFlit;
class RootFlit;

/** Pure virtual base encoder class.
 *
 *  Used to generate packets from headers.
 *
 *  The basic packet structure is
 *      SEQ_NUM   0 1 2 .. N N+1 .. N+C-1
 *      FLIT_TYPE H B B .. B CB  .. CT
 *  where H, B, and T is for Header, Body, and Tail, CB and CT is for a
 *  Code Body and Code Tail, N is the number of data flits and C is the
 *  number of redundant flits
 *
 *  The implementation of the encoding scheme is done by the derived
 *  classes.
 *
 *  The encoder also adjusts the timeout of the header messages taking the
 *  current packet's length into account.
 *
 */
class Encoder : public cSimpleModule
{
public:
    Encoder();
    virtual ~Encoder();

protected:
    virtual void initialize() override;
    virtual void finish() override;
    virtual void handleMessage(cMessage *msg) override;

protected:
    HeadFlit *m_head;   ///< Buffer for current packet's header.
    RootFlit *m_flit;   ///< Buffer for current flit.
    simtime_t m_t_clk;  ///< Clock period (used for repetition coding).
    int m_num_baseflit; ///< Current packet packet length.
    int m_num_codeflit; ///< Current packet redundancy length
    /// Threshold on packet length below which no code is applied.
    int m_encoding_length_threshold;
    int m_router_release_mode; ///< Resource release mode of routers.
    int m_blocksize;           ///< Code blocksize.

protected:
    void getHeadProperties(HeadFlit *head);         ///< get header properties
    virtual void setHeadProperties(HeadFlit *head); ///< set header properties

    virtual void encode() = 0; ///< interface to subclasses

    //** core functions for sending packets */
    void sendSequence(int num_flit, bool last_flit_tail = true);
    void sendFlits(int num_flit, int offset = 0, bool is_tail = false);
    //    BaseFlit* generateFlit(int sequence_num, bool is_tail = false);
    void generateFlit(int sequence_num, bool is_tail = false);
    void sendRepeatedHeads(const int num_heads);
};

/** Uncoded encoder.
 *	Basically the packet generator.
 */
class UncodedEncoder : public Encoder
{
protected:
    virtual void encode() override;
};

/** Simple encoder.
 *  Basically the packet generator.
 */
class SimpleEncoder : public Encoder
{
protected:
    virtual void encode() = 0;
    void encode(const char *encoder_name);
    virtual void setHeadProperties(HeadFlit *head) override;
};

/** Cyclic encoder.
 *  Cyclic repetition of flits.
 */
class CyclicEncoder : public Encoder
{
protected:
    virtual void encode() override;
};

/** Dynamic encoder.
 *  In network worm extension using registers.
 */
class DynamicEncoder : public SimpleEncoder
{
protected:
    virtual void encode() override;
};

/** FEC encoder.
 *  Optimal erasure code.
 */
class FecEncoder : public SimpleEncoder
{
protected:
    virtual void encode() override;
};

/** Single heuristic encoder.
 *  Code designed for independent erasures.
 */
class SingleHeurEncoder : public SimpleEncoder
{
protected:
    virtual void encode() override;
};

/** Burst heuristic encoder.
 *  Code designed for bursty ersasures.
 */
class BurstHeurEncoder : public SimpleEncoder
{
protected:
    virtual void encode() override;
};

/** Joint heuristic encoder.
 *  Code designed for independent and bursty erasures.
 */
class JointHeurEncoder : public SimpleEncoder
{
protected:
    virtual void encode() override;
};

/** Interleaved Block XOR Encoder.
 *
 */
class InterleavedBlockXEncoder : public SimpleEncoder
{
protected:
    virtual void setHeadProperties(HeadFlit *head) override;
    virtual void encode() override;
};

#endif
