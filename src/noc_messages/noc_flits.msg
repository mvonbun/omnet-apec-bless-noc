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
// File      : noc_flits.msg
// Created   : 15 Mar 2017 for apec_bless_noc
// Author(s) : Michael Vonbun
// E-mail(s) : michael.vonbun@tum.de
// 

// IMPORTANT: from 6.2.9 Inheritance
// ... using the extends keyword (only single inheritance is supported):

// FOR CONSISTENCY:
// 	- set the message / packet types whenever you create the messages 
//    (i.e. do not statically set them here, as some kinds can be set here, 
//     but some must either way be set at runtime / inside the modules)
//		

// include generated enum definitions
cplusplus {{
#include <vector>
#include <omnetpp.h>
#include "enum_definitions_m.h"
typedef std::vector<int> IntVector;
typedef std::vector<omnetpp::simtime_t> SimtimeVector;
}};

class noncobject IntVector;
class noncobject SimtimeVector;

enum CodeTypes;

// Hierarchy:
// RootFlit
// | - HeadFlit
// | - AckFlit
// | - BodyFlit
// | - TailFlit

//
// Root flit (base for all flit types).
//
packet RootFlit {
    schedulingPriority = SP_BASE;
    int sequence_number;
} 


//
// Body flit.
//
packet BodyFlit extends RootFlit {
    @customize(true);
    kind = BODY;
}


//
// Tail flit.
//
packet TailFlit extends RootFlit {
    @customize(true);
    kind = TAIL;
}


//
// Header flit.
//
packet HeadFlit extends RootFlit {
    @customize(true);
    // set head properties
    kind = HEAD;
	sequence_number = 0;
    
    // control info
    int source;
    int destination;
    
    // packet indentification
    simtime_t generation_time;
    simtime_t injection_time;
    long packet_id;  // id of a packet
    long message_id; // id of a message (== multiple packets)
    int priority;    // packet priority
    
    // packet size
    int num_flit;     // total number of flits
    int num_baseflit; // number of payload flits
    int num_codeflit; // number of code flits
    
    // transmission properties
    simtime_t timeout; // timeout until retransmission
    int attempts;      // number of transmission attempts
    // int num_hops;      // distance traveled
    
    // properties for statistics
    int distance;
}


//
// Ack.
//
packet AckFlit extends RootFlit {
    @customize(true);
    // set ack properties
    schedulingPriority = SP_EARLIER;
    kind = ACK;
    sequence_number = 0;
    
    // control info
    int source;
    int destination;
    
    // ack properties
    simtime_t generation_time;
    simtime_t injection_time;

    // acked packet properties
    int packet_priority;  // packet priority
    long packet_id;       // packet id
    int packet_attempts;  // attempts so far
    int packet_num_flit;  // packet size
    simtime_t packet_generation_time;
    simtime_t packet_injection_time;
}

//
// Nack.
//
packet NackFlit extends RootFlit {
 	@customize(true);
 	// set nack properties
    schedulingPriority = SP_EARLIER;
    kind = NACK;
    sequence_number = 0;

    // control info
    int source;
    int destination;

    // nack properties
    simtime_t generation_time;
    simtime_t injection_time;

    // nacked packet properties
    int packet_priority;  // packet priority
    long packet_id;       // packet id
    int packet_attempts;  // attempts so far
    int packet_num_flit;  // packet size
    simtime_t packet_generation_time;
    simtime_t packet_injection_time;

    // nack missing data notification capability
    bool carries_decoder_info; // indicate if the nack carries information of missing flits
    IntVector missing_flits;   // decoder info of missing flits

}

//
// Combined Nack.
//
packet CNackFlit extends RootFlit {
    @customize(true);
    // set nack properties
    schedulingPriority = SP_EARLIER;
    kind = CNACK;
    sequence_number = 0;
    int num_nacks_combined = 0;
}
