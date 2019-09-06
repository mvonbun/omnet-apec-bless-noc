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
// File      : encoder.ned
// Created   : 21 Mar 2017 for apec_bless_noc
// Author(s) : Michael Vonbun
// E-mail(s) : michael.vonbun@tum.de
// 

package apec_bless_noc.encoders;

//
// interface to the encoders
// 	note that the base encoder is pure virtual and therefore can not be a
// module
moduleinterface Encoder
{
parameters:
    // Parameters.
    double t_clk @unit(s);      // Clock period.
    int num_codeflit;           // Number of code flits.
    int length_threshold;       // Packet length threshold below which no code is applied.
    string router_release_mode; // Resource release mode of routers.
    int blocksize;              // Number of flits that are considered one block (for codes that use blocks)

    // Style.
    @display("i=block/encrypt");
gates:
    input in;
    output out;
}

simple UncodedEncoder like Encoder
{
parameters:
    // Parameter defaults.
    double t_clk @unit(s) = default(1s);
    int num_codeflit = default(0);
    int length_threshold = default(0);
    string router_release_mode = default("TAILFLAG");
    int blocksize = default(0);
    
    // Statistics signals.
gates:
    input in;
    output out;
}

simple CyclicEncoder extends UncodedEncoder { @class(CyclicEncoder); }

simple DynamicEncoder extends UncodedEncoder { @class(DynamicEncoder); }

simple FecEncoder extends UncodedEncoder { @class(FecEncoder); }

simple SingleHeurEncoder extends UncodedEncoder { @class(SingleHeurEncoder); }

simple BurstHeurEncoder extends UncodedEncoder { @class(BurstHeurEncoder); }

simple JointHeurEncoder extends UncodedEncoder { @class(JointHeurEncoder); }

simple InterleavedBlockXEncoder extends UncodedEncoder { @class(InterleavedBlockXEncoder); }
