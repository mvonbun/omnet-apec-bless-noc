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
// File      : ctrl.ned
// Created   : 05 Apr 2019 for apec_bless_noc
// Author(s) : Michael Vonbun
// E-mail(s) : michael.vonbun@tum.de
// 

package apec_bless_noc.sim_ctrl;

//
// TODO auto-generated module
//
simple Ctrl
{
    parameters:
        int num_nodes = default(1);
        double converge_threshold = default(0.01);
                
        // Style.
        @display("i=block/cogwheel");
    
	gates:
        input port @directIn;
}
