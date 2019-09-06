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
// File      : decoder.ned
// Created   : 21 Mar 2017 for apec_bless_noc
// Author(s) : Michael Vonbun
// E-mail(s) : michael.vonbun@tum.de
// 

package apec_bless_noc.decoders;

// interface to the decoders
//  note that the base decoder is pure virtual and therefore can not be a module
moduleinterface Decoder
{
parameters:
    // Parameters.
    int id;                        // module id
    double t_clk @unit(s);         // clock period
    int num_nodes;                 // number of network nodes
    string release_mode;           // release mode
    string ack_mode;               // ack or nack acknowledgments
    int blocksize;                 // code blocksize
    bool aggregate_en;             // enable aggregation
    
    // Style.
    @display("i=block/decrypt");
gates:
    inout port; // to nio
    output out; // to sink
}

//
// uncoded decoder
//
simple UncodedDecoder like Decoder
{
parameters:
    // Parameter defaults.
    int id = default(0);
    double t_clk @unit(s) = default(100ms);
    int num_nodes = default(0);
    string release_mode = default("Flag");
    string ack_mode = default("Ack");
    int blocksize = default(0);
    bool aggregate_en = default(false);
    
    // policy when the ack is sent:
    // -1: when tail arrives
    //  0: off / immediately
    //  n: after n cycles (n > 0)
    int ack_delay_policy = default(0);
    // control how latency is computed
    //   1: only when entire packet was received <=> application
    //   0: as-early-as-possible <=> non-application
    int latency_policy = default(1);

    // Statistics signals.
    // Packet length.
    @signal[dec-*_num_flit](type=long);
    @statisticTemplate[dec_num_flit](record=count,sum,mean,timeavg,vector?);

    // Multiple received flits.
    @signal[dec-*_num_flit_rx_before](type=long);
    @statisticTemplate[dec_num_flit_rx_before](record=count,mean,sum,timeavg,vector?);

gates:
    inout port; // to ni
    output out; // to sink
}

//
// simple decoder
//
simple SimpleDecoder like Decoder
{
parameters:
    // Parameter defaults.
    int id = default(0);
    double t_clk @unit(s) = default(100ms);
    int num_nodes = default(0);
    string release_mode = default("Flag");
    string ack_mode = default("Ack");
    int blocksize = default(0);
    bool aggregate_en = default(false);
    
    // Statistics signals.
    // Packet length (payload + code).
    @signal[dec-*_num_flit](type=long);
    @statisticTemplate[dec_num_flit](record=count,sum,mean,timeavg,vector?);

    // Multiple received flits.
    @signal[dec-*_num_flit_rx_before](type=long);
    @statisticTemplate[dec_num_flit_rx_before](record=count,mean,sum,timeavg,vector?);

    // Real number of flits received (head-to-tail, i.e. payload + code - holes).
    @signal[dec-*_num_flit_received](type=unsigned long);
    @statisticTemplate[dec_num_flit_received](record=count,sum,mean,timeavg,histogram,vector?);

    // Packet payload.
    @signal[dec-*_num_flit_needed](type=long);
    @statisticTemplate[dec_num_flit_needed](record=count,sum,mean,timeavg,histogram,vector?);
    
    // Number of flits of a nondecodable packet.
    @signal[dec-*_num_flit_nondecodable](type=long);
    @statisticTemplate[dec_num_flit_nondecodable](record=count,sum,mean,timeavg,histogram,vector?);

    // Number of missing flits of a nondecodable packet.
    @signal[dec-*_num_flit_missing](type=long);
    @statisticTemplate[dec_num_flit_missing](record=count,sum,mean,timeavg,histogram,vector?);

gates:
    inout port; // to ni
    output out; // to sink
}

//
// basic decoders
//
simple CyclicDecoder extends SimpleDecoder { @class(CyclicDecoder); }

simple DynamicDecoder extends SimpleDecoder { @class(DynamicDecoder); }

simple FecDecoder extends SimpleDecoder { @class(FecDecoder); }

simple InterleavedBlockXDecoder extends SimpleDecoder { @class(InterleavedBlockXDecoder); }

//
// heuristic decoders
//
simple SingleHeurDecoder extends SimpleDecoder { @class(SingleHeurDecoder); }

simple BurstHeurDecoder extends SimpleDecoder { @class(BurstHeurDecoder); }

simple JointHeurDecoder extends SimpleDecoder { @class(JointHeurDecoder); }

//
// basic aggregation decoders
//
simple AggregateCyclicDecoder extends SimpleDecoder
{
    @class(AggregateCyclicDecoder);
}

simple AggregateDynamicDecoder extends SimpleDecoder
{
    @class(AggregateDynamicDecoder);
}

simple AggregateFecDecoder extends SimpleDecoder
{
    @class(AggregateFecDecoder);
}

simple AggregateInterleavedBlockXDecoder extends SimpleDecoder
{
    @class(AggregateInterleavedBlockXDecoder);
}
//
// heuristic aggregation decoders
//
simple AggregateSingleHeurDecoder extends SimpleDecoder
{
    @class(AggregateSingleHeurDecoder);
}

simple AggregateBurstHeurDecoder extends SimpleDecoder
{
    @class(AggregateBurstHeurDecoder);
}

simple AggregateJointHeurDecoder extends SimpleDecoder
{
    @class(AggregateJointHeurDecoder);
}

// moduleinterface Decoder
// {
//     parameters:
//         double t_clk @unit(s);
//         @display("i=block/decrypt");
//     gates:
//         inout port; // to ni
//         output out; // to sink
// }

// module Decoder_tailFlag like Decoder
// {
//     parameters:
//         double t_clk @unit(s) = default(100ms); // clock period
//         @display("i=block/decrypt");
//     gates:
//         inout port; // to ni
//         output out; // to sink
// }

// module Decoder_packetLength like Decoder
// {
//     parameters:
//         double t_clk @unit(s) = default(100ms); // clock period
//         @display("i=block/decrypt");
//     gates:
//         inout port; // to ni
//         output out; // to sink
// }

//
// TODO auto-generated module
//
// simple Decoder
// {
//    parameters:
//        double t_clk @unit(s) = default(100ms); // clock period
//        bool use_tail_flag = default(true); // resource release mechanism
//        @display("i=block/decrypt");
//
//    gates:
//        inout port; // to ni
//        output out; // to sink
// }
