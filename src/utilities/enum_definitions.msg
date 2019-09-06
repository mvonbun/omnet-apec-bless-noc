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
// File      : enum_definitions.msg
//
// Created   : 15 Mar 2017 for apec_bless_noc
// Author(s) : Michael Vonbun
// E-mail(s) : michael.vonbun@tum.de
// 

//
// message types
// flit messge types can also be used as priorities (the higher the better)
//
enum MessageTypes {
    EVENT    = 0;  // base event
    TXEVENT  = 1;  // event triggering transmission
    TIMEOUT  = 2;  // timeout event
    ARBEVENT = 3;  // arbitration event
    RELEVENT = 4;  // resource release event
    PACKTRIG = 5;  // packet trigger message
    MONITOR  = 6;  // message for the monitor
    FLIT     = 20; // base flit (no event)
    HEAD     = 30; // Packet Header
    HEADTAIL = 31; // One flit packet (head and tail)
    ACK      = 40; // Acknowledgement message
    NACK     = 45; // NAcknowledgement message
    CNACK    = 46; // Combined Nack message
    DATA     = 50; // Packet Data (Body or Tail)
    BODY     = 51; // Packet Body
    TAIL     = 52; // Packet Tail
    CTRL     = 60; // Simulation control message
    CBUFOVFL = 61; // Pending buffer overflow
    CCONV    = 62; // Convergence message
};

//
// scheduling priorities
//
enum SchedulingPriorities {
    SP_FIRST      = 10;
    SP_VERY_EARLY = 20;
    SP_EARLY      = 30;
    SP_EARLIER    = 40;
    SP_BASE       = 50;
    SP_LATER      = 60;
    SP_LATE       = 70;
    SP_VERY_LATE  = 80;
    SP_LAST       = 90;
};

//
// priorities
// (lower is better; see sorting compare routines of routers)
// maybe for future: increase prio at every cycle
//
enum PacketPriorities {
    PRIO_PACKET_HIGHEST  = 10;
    PRIO_PACKET_VERYHIGH = 20;
    PRIO_PACKET_HIGHER   = 30;
    PRIO_PACKET_HIGH     = 40;
    PRIO_PACKET_BASE     = 50;
    PRIO_PACKET_LOW      = 60;
    PRIO_PACKET_LOWER    = 70;
    PRIO_PACKET_VERYLOW  = 80;
    PRIO_PACKET_LOWEST   = 90;
};

//
// code types
//
enum CodeTypes {
    UNCODED = 0;  // no code used
    CYCLIC  = 10; // cyclic repetition code
    DYNAMIC = 20; // dynamic recovery
    HEUR    = 30; // robustness against single collisions
    HEUR_S  = 31; // robustness against single collisions
    HEUR_B  = 32; // robustness against bursty collisions
    HEUR_J  = 33; // joint robustness against single and bursty collisions
    FEC     = 40; // optimal forward error correction (http://pages.cs.wisc.edu/~suman/courses/740/papers/rizzo97ccr.pdf)
    BLOCKX  = 50; // blockwise single xor
};

//
// link release mechanism
//
enum ResourceReleaseTypes {
    TAILFLAG  = 0; // use tail flag
    FLITCOUNT = 1; // use flit counter
};

//
// module type enumerations
//
enum ModuleTypes {
 	SOURCE = 0;
 	ENCODER = 10;
 	NI = 20;
 	ROUTER = 30;
 	DECODER = 40;
 	SINK = 50;
};



