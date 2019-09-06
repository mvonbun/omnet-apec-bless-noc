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
// File      : router.ned
// Created   : 15 Mar 2017 for apec_bless_noc
// Author(s) : Michael Vonbun
// E-mail(s) : michael.vonbun@tum.de
// 

package apec_bless_noc.routers;

moduleinterface Router
{
parameters:
    // Parameters.
    int id;                   // module id
    double t_clk @unit(s);    // clock period
    int num_rows;             // 2D-mesh number of rows
    int num_columns;          // 2D-mesh number of columns
    int row;                  // module's row
    int column;               // module's colum
    int delay;                // processing delay
    int num_ports;            // number of ports
    int num_states_threshold; // max number of port states expected
    
    // Style.
    @display("i=block/routing");
gates:
    inout port[num_ports];
}

simple BaseRouter like Router
{
parameters:
    // Parameter defaults.
    int id = default(0);                    // module id
    double t_clk @unit(s) = default(100ms); // clock period
    int num_rows = default(1);    // 2D-mesh number of rows
    int num_columns = default(1); // 2D-mesh number of columns 
    int row = default(0);         // module's row
    int column = default(0);      // module's colum
    int delay = default(1);       // processing delay
    int num_ports = default(5);	  // number of ports
    int num_states_threshold = default(1); // max number of port states expected
    
    // Statistics.
    @signal[router-*_flit-drop](type=bool);
    @statisticTemplate[router_flit-drop](record=count,mean,timeavg,histogram,vector?);
    
    @signal[router-*_ack-drop](type=long);
    @statisticTemplate[router_ack-drop](record=count,sum,mean,timeavg,histogram,vector?);
    
    @signal[router-*_packet-drop](type=long);
    @statisticTemplate[router_packet-drop](record=count,sum,mean,timeavg,histogram,vector?);
    
    @signal[router-*_packet-drop-distance](type=long);
    @statisticTemplate[router_packet-drop-distance](record=count,sum,mean,timeavg,histogram,vector?);
    
    @signal[router-*_nack-reroute](type=bool);
    @statisticTemplate[router_nack-reroute](record=count,mean,timeavg,histogram,vector?);
    
    @signal[router-*_cnack-number](type=long);
    @statisticTemplate[router_cnack-number](record=count,sum,mean,max,timeavg,histogram,vector?);
    
gates:
    inout port[num_ports];
}

simple PlainFlagAckRouter extends BaseRouter { @class(PlainFlagAckRouter); }

simple PlainFlagNackRouter extends BaseRouter { @class(PlainFlagNackRouter); }

simple PlainLengthAckRouter extends BaseRouter { @class(PlainLengthAckRouter); }

simple PlainLengthNackRouter extends BaseRouter { @class(PlainLengthNackRouter); }

simple CodedFlagAckRouter extends BaseRouter 
{
parameters:
    bool supplant_acks_only = default(false); // for flag router in [true, false]
    int supplant_threshold = default(1);     // for flag router in [0, 1]
    
    @signal[router-*_supplant](type=bool);
    @statisticTemplate[router_supplant](record=count,vector?);
    
    @class(CodedFlagAckRouter);
}

simple CodedFlagNackRouter extends CodedFlagAckRouter { @class(CodedFlagNackRouter); }

simple CodedLengthAckRouter extends CodedFlagAckRouter { @class(CodedLengthAckRouter); }

simple CodedLengthNackRouter extends CodedFlagAckRouter { @class(CodedLengthNackRouter); }

//simple SimpleHealingFlagRouter extends BaseRouter 
//{ 
//parameters:
//    @class(SimpleHealingRouter);
//    // Parameter defaults
//    int fifo_size = default(0); // size of intersecting flits to compensate
//}


