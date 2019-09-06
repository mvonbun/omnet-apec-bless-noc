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
// File      : tile.ned
// Created   : 15 Mar 2017 for apec_bless_noc
// Author(s) : Michael Vonbun
// E-mail(s) : michael.vonbun@tum.de
// 

package apec_bless_noc.modules;

// basic omnet modules
import ned.DelayChannel;
// project modules
import apec_bless_noc.sources.Source;
import apec_bless_noc.sinks.Sink;
import apec_bless_noc.testing.Terminator;
import apec_bless_noc.encoders.*Encoder;
import apec_bless_noc.decoders.*Decoder;
import apec_bless_noc.network_interfaces.*NetworkInterface;

module Tile
{
    parameters:
        // General.
        double t_clk @unit(s) = default(1s);
        int id = default(0);
        
        // Network.
        int num_nodes = default(0);   // number of nodes
        int num_rows = default(1);    // number of rows
        int num_columns = default(1); // number of columns
        
        // Module identification.
        int row = default(0);    // module's row
        int column = default(0); // module's colum
        
        // Submodule properties.
        string code_type = default("Uncoded"); // [Uncoded, Cyclic, Dynamic, Fec, SingleHeur, BurstHeur, JointHeur, InterleavedBlockX]
        string decoder_mode = default("");     // ["", "Aggregate"]
        string ni_type = default("Blocking");  // ["Blocking", "Nonblocking"]
        string release_mode = default("Flag"); // ["Flag", "Length"]
        string ack_mode = default("Ack");      // ["Ack", "Nack"]
        int blocksize = default(0);            // block wise code blocksize
        bool aggregate_en = default(false);    // decoder mode
        bool end_simulation_on_full_buffer = default(false); // realistic pending buffer treatment
        int link_delay = default(1);
        
        // Style.
        @display("i=block/circle");
        
    gates:
        inout port;
        
    submodules:
        source: Source {
            t_clk = t_clk;
            id = id;
            num_rows = num_rows;
            num_columns = num_columns;
            row = row;
            column = column;
            ack_mode = ack_mode;
            link_delay = link_delay;
        };
        encoder: <code_type + "Encoder"> like Encoder {
            t_clk = t_clk;
            router_release_mode = release_mode;
            blocksize = blocksize;
        };
        ni: <ni_type + release_mode + ack_mode + "NetworkInterface"> like NetworkInterface
        {
            t_clk = t_clk;
            id = id;
            end_simulation_on_full_buffer = end_simulation_on_full_buffer;
        };
        decoder: <code_type + "Decoder"> like Decoder {
            t_clk = t_clk;
            id = id;
            num_nodes = num_nodes;
            release_mode = release_mode;
            ack_mode = ack_mode;
            blocksize = blocksize;
            aggregate_en = aggregate_en;
        };
        sink: Sink {
            t_clk = t_clk;
            id = id;
        };

    connections:
        source.out --> encoder.in;
        encoder.out --> ni.source;
        ni.port <--> ned.DelayChannel { delay=t_clk; } <--> port; // add a one cycle register delay
        ni.sink <--> decoder.port;
        decoder.out --> sink.in;
}
