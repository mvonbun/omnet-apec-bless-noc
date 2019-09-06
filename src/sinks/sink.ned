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
// File      : sink.ned
// Created   : 08 Apr 2017 for apec_bless_noc
// Author(s) : Michael Vonbun
// E-mail(s) : michael.vonbun@tum.de
// 

package apec_bless_noc.sinks;

simple Sink
{
parameters:
    // Parameters.
    int id = default(0); 					// module id
    double t_clk @unit(s) = default(100ms); // clock period
    double convergence_check_interval @unit(s) = default(0s); // interval of checking for convergence (> 0 to enable)
    
    // Statistics.
    @signal[sink-*_latency_generation_cycles](type=long);
    @statisticTemplate[sink_latency_generation_cycles](record=count,sum,mean,max,timeavg,histogram,vector?);

    @signal[sink-*_latency_injection_cycles](type=long);
    @statisticTemplate[sink_latency_injection_cycles](record=count,sum,mean,max,timeavg,histogram,vector?);

    @signal[sink-*_latency_network_access_cycles](type=long);
    @statisticTemplate[sink_latency_network_access_cycles](record=count,sum,mean,max,timeavg,histogram,vector?);

    @signal[sink-*_attempts](type=long);
    @statisticTemplate[sink_attempts](record=count,sum,mean,max,timeavg,histogram,vector?);

    // Style.
    @display("i=block/sink,coral2");
    
gates:
    input in;
    input abort_simulation_port @directIn;
}
