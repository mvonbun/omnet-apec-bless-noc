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
// File      : base_code.cc
// Created   : 03 Apr 2017 for apec_bless_noc
// Author(s) : Michael Vonbun
// E-mail(s) : michael.vonbun@tum.de
// 

#include "base_code.h"
#include <limits>
#include <stdexcept>

#define S1(x) #x
#define S2(x) S1(x)
#define LOCATION(x) __FILE__ ":" S2(__LINE__) ": " x

// update receive sequence bitvector
void BaseCode::received(int i, unsigned int &received)
{
    if (i < std::numeric_limits<unsigned int>::digits) {
        received |= (1 << i);
    } else {
        throw std::out_of_range(
            LOCATION("Receive sequence bitvector has too few bits."));
    }
}

// hamming weight
int BaseCode::hammingWeight(unsigned int a)
{
    int z = 0;
    while (a > 0) {
        ++z;
        a &= a - 1;
    }
    return z;
}
