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
// File      : heuristic_code.cc
// Created   : 03 Apr 2017 for apec_bless_noc
// Author(s) : Michael Vonbun
// E-mail(s) : michael.vonbun@tum.de
// 

#include "heuristic_code.h"
#include <cassert>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <iterator>
#include <stdexcept>

// #define OMNET
#ifndef OMNET
#define LOADPATH "./"
#else
#define LOADPATH "../src/code/"
#endif

#define S1(x) #x
#define S2(x) S1(x)
// #define EXCEPTION(x) __FILE__ ":" S2(__LINE__) ": " x
#define LOCATION __FILE__ ":" S2(__LINE__) ": "

// read syndrome vector from file
std::vector<unsigned int> HeuristicCode::readIntoMemberFromFile()
{

    // get filename to read data
    char filename[100];
    // /home/mivo/dev/noc/omnet/bufferless/apec_bless_noc/simulations
    sprintf(filename, "%s%s/%02d_%02d_%s.dat", LOADPATH, m_codetype.c_str(),
            m_num_baseflit, m_num_codeflit, "dec");

    // open and vaildate file stream
    std::ifstream fid(filename);
    if (!fid.is_open()) {
        char error_msg[200];
        sprintf(error_msg, "%s%s: %s", LOCATION, "No such file", filename);
        throw std::runtime_error(error_msg);
    }

    std::istream_iterator<int> start(fid), end;
    std::vector<unsigned int> numbers(start, end);

    return numbers;
}

// check if received bitsequence is decodable
bool HeuristicCode::decodable(unsigned int received)
{
    return decodable(m_num_baseflit, received); // relay convenience function
}
bool HeuristicCode::decodable(const int num_baseflit,
                              const unsigned int received)
{

    // check if enough flits have been received
    if (hammingWeight(received) < num_baseflit) {
        return false;

    } else {
        // loop through syndrome vector and check if the receive sequence
        // matches any syndrome
        // a syndrome here is a receive sequence that is not decodable
        bool is_decodable = true;
        for (size_t i = 0; ((is_decodable) && (i < m_syndrome.size())); ++i) {
            is_decodable = !(received == m_syndrome[i]);
        }
        return is_decodable;
    }
}
