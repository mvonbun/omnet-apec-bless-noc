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
// File      : source.ned
// Created   : 12 Apr 2017 for apec_bless_noc
// Author(s) : Michael Vonbun
// E-mail(s) : michael.vonbun@tum.de
// 

package apec_bless_noc.sources;

//
// Data source.
// Using the defaults we get generation rate of 0.1 flits/cycle/tile
// as we inject 5 flits per packet on average every 50 cycles.
// 
simple Source
{
parameters:
    // Generic.
    double t_clk @unit(s) = default(1s); // clock period
    int id = default(0);                 // module network id

    // Network.
    int num_rows = default(1);        // 2D-mesh number of rows
    int num_columns = default(1);     // 2D-mesh number of columns
    string ack_mode = default("Ack"); // ack or nack (for timeout)

    // Module identification.
    int row = default(0);    // module's row
    int column = default(0); // module's colum

    // Traffic generation.
    volatile double send_interval @unit(s) = default(exponential(50s)); 
    volatile int dest_id = default(-1); // -1 disables source
    volatile int priority = default(5); // PRIO_PACKET_BASE = 5 (see enum_definition.h)
    // Message and packet lengths.
    string traffic_pattern = default("Uniform");
    volatile int num_flit = default(intuniform(1, 9)); // [min, max]
    // Message mechanism.
    //   max packet length used to
    //   - determine queuing delay of ACK/NACK in ni
    //   - slice messages into packets
    bool message_en = default(false);
    int max_packet_length = default(0);

    // Packet timing.
    int router_delay = default(1);         // delay in cycles of a router / hop
    int link_delay = default(1);           // delay of the links
    int max_codeflits = default(0);        // maximum number of code flits added
    int timeout_guard_cycles = default(0); // prevent too early timeout trigger

    // Statistics.
    @signal[src-*_packet_length](type=long);
    @statisticTemplate[src_packet_length](record=count,sum,mean,max,timeavg,histogram,vector?);

    @signal[src-*_packet_destination](type=long);
    @statisticTemplate[src_packet_destination](record=histogram,vector?);

    @signal[src-*_packet_debug](type=long);
    @statisticTemplate[src_packet_debug](record=vector?);

    // Style.
    @display("i=block/source");

gates:
    output out;
    input trigger_port @directIn;
}
