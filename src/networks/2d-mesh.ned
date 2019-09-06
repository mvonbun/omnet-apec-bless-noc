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
// File      : 2d-mesh.ned
// Created   : 15 Mar 2017 for apec_bless_noc
// Author(s) : Michael Vonbun
// E-mail(s) : michael.vonbun@tum.de
// 

package apec_bless_noc.networks;

// basic omnet modules
import ned.DelayChannel;
import ned.IdealChannel;
// project modules
import apec_bless_noc.channels.ErasureChannel;
import apec_bless_noc.sources.Source;
import apec_bless_noc.testing.Terminator;
import apec_bless_noc.encoders.*Encoder;
import apec_bless_noc.decoders.*Decoder;
import apec_bless_noc.network_interfaces.*NetworkInterface;
import apec_bless_noc.modules.Tile;
import apec_bless_noc.routers.*Router;
import apec_bless_noc.sim_ctrl.Ctrl;

//
// A generated network with grid topology.
//
network Mesh {
    parameters:
        // network properties
        int num_columns = default(3);
        int num_rows = default(3);
        double t_clk @unit(s) = default(1s);
        string release_mode = default("Flag"); // ["Flag", "Length"]
        
        // injection rate to source send interval
        double generation_rate = default(0.01);
        int num_flit_min = default(1);
        int num_flit_max = default(8);
        string packet_length_distribution = default("Uniform"); // distribution of packet length
        
        // tile properties
        string code_type = default("Uncoded"); // [Uncoded, Cyclic, Dynamic, Fec, SingleHeur, BurstHeur, JointHeur, InterleavedBlockX]  
        string decoder_mode = default("");     // ["", "Aggregate"]
        string ni_type = default("Blocking");  // ["Blocking", "Nonblocking"]
        int blocksize = default(0);            // blocksize of en-/decode
        bool aggregate_en = default(false);    // decoder mode
        bool end_simulation_on_full_buffer = default(false); // realistic pending buffer treatment
        
        // router properties
        string router_type = default("Plain"); // ["Plain", "Coded", "Healing"]
        string ack_mode = default("Ack");      // ["Ack", "Nack"]
        int link_delay = default(1);           // delay in cycles of router links

    types:
        // channel Link extends ned.IdealChannel { }
        channel Link extends ned.DelayChannel { delay=(link_delay*t_clk); }
        channel TileLink extends ned.IdealChannel { }

    submodules:
        ctrl : Ctrl {
            num_nodes = num_columns * num_rows;
            @display("p=25,25");
        }
        
        tile[num_columns * num_rows] : Tile {
            t_clk = t_clk;
            num_nodes = num_columns * num_rows;
            num_rows = num_rows;
            num_columns = num_columns;
            
            id = index;
            row = index / num_columns;
            column = index % num_columns;
            
            code_type = code_type;
            decoder_mode = decoder_mode;
            ni_type = ni_type;
            release_mode = release_mode;
            ack_mode = ack_mode;
            blocksize = blocksize;
            aggregate_en = aggregate_en;
            end_simulation_on_full_buffer = end_simulation_on_full_buffer;
            link_delay = link_delay;
            
            // source.send_interval = exponential(((num_flit_min + num_flit_max) / (2*injection_rate))s);
            
            @display("p=75,75,m,$num_columns,100,100");
        }

        router[num_columns * num_rows] : <router_type + release_mode + ack_mode + "Router"> like Router {
	    t_clk = t_clk;
	    num_rows = num_rows;
	    num_columns = num_columns;

	    id = index;
	    row = index / num_columns;
	    column = index % num_columns;

	    delay = 1;
	    num_ports = 5;

	    @display("p=125,125,m,$num_columns,100,100");
        }
        
    connections allowunconnected:
        // clock-wise port enumeration
        // ([port][direction] : [0][local] [1][north] [2][east] [3][south] [4][west])
        // connect tiles first to port[0] of every router
        for i=0..num_rows-1, for j=0..num_columns-1 {
            tile[i * num_columns + j].port
            <--> TileLink <-->
            router[i * num_columns + j].port[0];
            // start connecting the routers: top-to-bottom, left-to-right
            router[i * num_columns + j].port[3]
            <--> Link <-->
            router[(i + 1) * num_columns + j].port[1] if i != num_rows - 1;
            router[i * num_columns + j].port[2]
            <--> Link <-->
            router[i * num_columns + j + 1].port[4] if j != num_columns - 1;
        }
}
