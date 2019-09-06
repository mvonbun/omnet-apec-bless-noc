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
// File      : noc_messages.msg 
// Created   : 15 Mar 2017 for apec_bless_noc
// Author(s) : Michael Vonbun
// E-mail(s) : michael.vonbun@tum.de
// 

// include generated enum definitions
cplusplus {{
#include "enum_definitions_m.h"
}}

enum CodeTypes;

//
// timeout message
//
message TimeoutMsg {
    @customize(true);
    kind = TIMEOUT;
    long packet_id;
    simtime_t timeout;
}

//
// release message
//
message ResourceReleaseMsg {
    @customize(true);
    kind = RELEVENT;
    long message_id;
    int port_id;
}

//
// packet generation trigger message
//
message PacketGenerationTriggerMsg {
 	kind = PACKTRIG;
 	int num_base_flit;
 	int destination;
 	int priority;
 	num_base_flit = -1;
 	destination = -1;
 	priority = -1;
}

//
// monitor message
//
message MonitorMsg {
    kind = MONITOR;
    int module_id;
    int module_type;
    int value;
}

//
// pending buffer overflow message
//
message PBuffOverflowMsg {
    @customize(true);
 	kind = CBUFOVFL;
 	schedulingPriority = SP_LAST;
 	int module_id;
}

//
// convergence message
//
message ConvergenceMsg {
    @customize(true);
 	kind = CCONV;
 	schedulingPriority = SP_LAST;
 	int module_id;
 	simtime_t avg_latency;
}


