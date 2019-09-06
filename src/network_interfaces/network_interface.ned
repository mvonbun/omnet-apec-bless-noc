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
// File      : network_interface.ned
// Created   : 11 Apr 2017 for apec_bless_noc
// Author(s) : Michael Vonbun
// E-mail(s) : michael.vonbun@tum.de
// 

package apec_bless_noc.network_interfaces;

moduleinterface NetworkInterface
{
parameters:
    // General.
    int id;
    double t_clk @unit(s);
    int max_pending_packets;
    bool late_ack_compensation;
    bool end_simulation_on_full_buffer;
    
    // Style.
    @display("i=noc/ni");
gates:
    inout port;   // to network
    input source; // to encoder / source
    inout sink;   // to decoder / sink
}

simple BlockingFlagAckNetworkInterface like NetworkInterface
{
parameters:
    // Parameter defaults.
    int id = default(0);                    // module id
    double t_clk @unit(s) = default(100ms); // clock period

    // Maximum number of packets pending in the retransmission buffer.
    int max_pending_packets = default(25);
    // Late ack compensation.
    // If true, packets enqueued in the output buffer are deleted when the
    // respective ACK arrives.
    bool late_ack_compensation = default(true);
    bool end_simulation_on_full_buffer = default(false);
    
    // Statistics signals
    // Number of packets / flits pending in the retransmission buffer.
    @signal[ni-*_pendingBuf_num_packet](type=unsigned long);
    @statisticTemplate[ni_pendingBuf_num_packet](record=count,sum,mean,max,timeavg,histogram,vector?);
    @signal[ni-*_pendingBuf_num_flit](type=unsigned long);
    @statisticTemplate[ni_pendingBuf_num_flit](record=count,sum,mean,max,timeavg,histogram,vector?);

    // Number of flits in the packet and ack output / transmission buffer.
    @signal[ni-*_outputQ_num_packet_flit](type=long);
    @statisticTemplate[ni_outputQ_num_packet_flit](record=count,sum,mean,max,timeavg,histogram,vector?);
    @signal[ni-*_outputQ_num_acks](type=long);
    @statisticTemplate[ni_outputQ_num_acks](record=count,sum,mean,max,timeavg,histogram,vector?);

    // Record @ACK reception.
    // Number of attempts.
    @signal[ni-*_packet_attempts](type=long);
    @statisticTemplate[ni_packet_attempts](record=count,sum,mean,max,timeavg,histogram,vector?);
    // Latency between packet generation / injection and ACK reception.
    @signal[ni-*_packet_latency_generation_cycles](type=long);
    @statisticTemplate[ni_packet_latency_generation_cycles](record=count,sum,mean,max,timeavg,histogram,vector?);
    @signal[ni-*_packet_latency_injection_cycles](type=long);
    @statisticTemplate[ni_packet_latency_injection_cycles](record=count,sum,mean,max,timeavg,histogram,vector?);
    // Latency between ACK generation / injection and ACK reception.
    @signal[ni-*_ack_latency_generation_cycles](type=long);
    @statisticTemplate[ni_ack_latency_generation_cycles](record=count,sum,mean,max,timeavg,histogram,vector?);
    @signal[ni-*_ack_latency_injection_cycles](type=long);
    @statisticTemplate[ni_ack_latency_injection_cycles](record=count,sum,mean,max,timeavg,histogram,vector?);
    @signal[ni-*_ack_latency_network_access_cycles](type=long);
    @statisticTemplate[ni_ack_latency_network_access_cycles](record=count,sum,mean,max,timeavg,histogram,vector?);
    
    // Latency between NACK generation / injection and NACK reception.
    @signal[ni-*_nack_latency_generation_cycles](type=long);
    @statisticTemplate[ni_nack_latency_generation_cycles](record=count,sum,mean,max,timeavg,histogram,vector?);

    // Number of flits injected into the network.
    @signal[ni-*_num_flits_injected](type=bool);
    @statisticTemplate[ni_num_flits_injected](record=count,sum,timeavg,histogram,vector?);
    @signal[ni-*_num_ack_injected](type=bool);
    @statisticTemplate[ni_num_ack_injected](record=sum,histogram,vector?);
    @signal[ni-*_num_packet_injected](type=long);
    @statisticTemplate[ni_num_packet_injected](record=count,sum,mean,timeavg,histogram,vector?);

    // Number of new (payload) flits pushed to the pending / retransmission buffer.
    @signal[ni-*_flit_generation_rate_payload](type=long);
    @statisticTemplate[ni_flit_generation_rate_payload](record=count,sum,mean,timeavg,histogram,vector?);
    @signal[ni-*_flit_generation_rate](type=long);
    @statisticTemplate[ni_flit_generation_rate](record=count,sum,mean,timeavg,histogram,vector?);

    // Number of ACKs that arrived after a packet timeout.
    @signal[ni-*_num_late_acks](type=long);
    @statisticTemplate[ni_num_late_acks](record=count,sum,mean,timeavg,histogram,vector?);

gates:
    inout port;   // to network
    input source; // to encoder / source
    inout sink;   // to decoder / sink
}

// Blocking Length is the same as Blocking Flag, module is just needed as ni is set up in tile.ned
simple BlockingFlagNackNetworkInterface extends BlockingFlagAckNetworkInterface
{
    @class(BlockingFlagNackNetworkInterface);
}

simple BlockingLengthAckNetworkInterface extends BlockingFlagAckNetworkInterface
{
    @class(BlockingFlagAckNetworkInterface);
}

simple BlockingLengthNackNetworkInterface extends BlockingFlagAckNetworkInterface
{
    @class(BlockingFlagNackNetworkInterface);
}


// Preserving mode
simple NonblockingFlagAckNetworkInterface extends BlockingFlagAckNetworkInterface
{
    @class(NonblockingFlagAckNetworkInterface);
}

simple NonblockingFlagNackNetworkInterface extends BlockingFlagAckNetworkInterface
{
    @class(NonblockingFlagNackNetworkInterface);
}

// Nonpreserving mode
simple NonblockingLengthAckNetworkInterface extends BlockingFlagAckNetworkInterface
{
    @class(NonblockingLengthAckNetworkInterface);
}

simple NonblockingLengthNackNetworkInterface extends BlockingFlagAckNetworkInterface
{
    @class(NonblockingLengthNackNetworkInterface);
}
