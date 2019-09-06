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

#include "heuristic_code_bloated.h"
#include <cassert>
#include <climits>
#include <cstring>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#define BURSTDIR "burst"
#define SINGLEDIR "single"
#define JOINTDIR "joint"
#define ENCEXT "enc"
#define DECEXT "dec"
#define STR(s) #s
#define XSTR(s) STR(s)
#define DEBUG(s) (std::cout << s << std::endl)
// #define DEBUG(s)

// public methods
// check if received sequence is decodable
bool HeuristicCodeBloat::decodable(unsigned int received)
{
    assert(received < UINT_MAX);
    if (hammingWeight(received) < m_num_baseflit) {
        return false;
    } else {
        bool is_decodable = true;
        // DEBUG(m_syndrome.size());
        for (size_t i = 0; ((is_decodable) && (i < m_syndrome.size())); ++i) {
            // DEBUG(i);
            is_decodable = !(received == m_syndrome[i]);
        }
        return is_decodable;
    }
}

std::vector<unsigned int> HeuristicCodeBloat::getCode() { return m_codewords; }
unsigned int HeuristicCodeBloat::getCodeWord(size_t i)
{
    return m_codewords[i];
}
size_t HeuristicCodeBloat::codeSize() { return m_codewords.size(); }

std::vector<unsigned int> HeuristicCodeBloat::getSyndrome()
{
    return m_syndrome;
}
unsigned int HeuristicCodeBloat::getSyndromeWord(size_t i)
{
    return m_syndrome[i];
}
size_t HeuristicCodeBloat::syndromeSize() { return m_syndrome.size(); }

void HeuristicCodeBloat::printCodeInfo()
{
    std::cout << m_codetype << "(" << m_num_baseflit << "," << m_num_codeflit
              << ")" << std::endl;
}

void HeuristicCodeBloat::printGeneratorMatrix()
{
    unsigned int bitmask = 0;

    std::cout << "G = " << std::endl;

    for (size_t i = 0; i < m_codewords.size(); ++i) {
        bitmask = 1;
        std::cout << "    ";
        for (int k = 1; k < m_num_baseflit; ++k) {
            if (bitmask & m_codewords[i]) {
                std::cout << "1 ";
            } else {
                std::cout << "0 ";
                ;
            }
            bitmask <<= 1;
        }
        std::cout << std::endl;
    }
}

std::vector<unsigned int> HeuristicCodeBloat::getFlitsInCodeWord(size_t k)
{
    unsigned int bitmask = 1;
    std::vector<unsigned int> result;
    for (int i = 0; i < m_num_baseflit; ++i) {
        if (bitmask & m_codewords[k]) {
            result.push_back((unsigned int)i);
        }
        bitmask <<= 1;
    }
    return result;
}

// private methods
// data retrieval core
std::vector<unsigned int> HeuristicCodeBloat::readFromFile(bool enc)
{
    char filename[100];
    //    /home/mivo/dev/noc/omnet/bufferless/apec_bless_noc/simulations
    sprintf(filename, "../src/codes/%s/%02d_%02d_%s.dat", m_codetype.c_str(),
            m_num_baseflit, m_num_codeflit, (enc) ? (ENCEXT) : (DECEXT));

    // DEBUG
    // std::cout << filename << std::endl;
    DEBUG(filename);

    std::ifstream fid(filename);

    assert(fid.is_open());
    std::istream_iterator<int> start(fid), end;
    std::vector<unsigned int> numbers(start, end);
    return numbers;
}

// data retrieval core interface
std::vector<unsigned int> HeuristicCodeBloat::retrieve(bool enc,
                                                       const char *codetype,
                                                       int num_baseflit,
                                                       int num_codeflit)
{
    return retrieve(enc, std::string(codetype), num_baseflit, num_codeflit);
}
std::vector<unsigned int>
HeuristicCodeBloat::retrieve(bool enc, const std::string &codetype,
                             int num_baseflit, int num_codeflit)
{
    m_num_baseflit = num_baseflit;
    m_num_codeflit = num_codeflit;
    return retrieve(enc, codetype);
}
std::vector<unsigned int> HeuristicCodeBloat::retrieve(bool enc,
                                                       const char *codetype)
{
    return retrieve(enc, std::string(codetype));
}
std::vector<unsigned int>
HeuristicCodeBloat::retrieve(bool enc, const std::string &codetype)
{
    m_codetype = codetype;
    return readFromFile(enc);
}

// data retrieval encoding interface
std::vector<unsigned int>
HeuristicCodeBloat::retrieveEncoding(const char *codetype, int num_baseflit,
                                     int num_codeflit)
{
    return retrieve(true, codetype, num_baseflit, num_codeflit);
}
std::vector<unsigned int>
HeuristicCodeBloat::retrieveEncoding(const char *codetype)
{
    return retrieve(true, codetype);
}
std::vector<unsigned int>
HeuristicCodeBloat::retrieveEncoding(const std::string &codetype,
                                     int num_baseflit, int num_codeflit)
{
    return retrieve(true, codetype, num_baseflit, num_codeflit);
}
std::vector<unsigned int>
HeuristicCodeBloat::retrieveEncoding(const std::string &codetype)
{
    return retrieve(true, codetype);
}

// data retrieval decoding interface
std::vector<unsigned int>
HeuristicCodeBloat::retrieveDecoding(const char *codetype, int num_baseflit,
                                     int num_codeflit)
{
    return retrieve(false, codetype, num_baseflit, num_codeflit);
}
std::vector<unsigned int>
HeuristicCodeBloat::retrieveDecoding(const char *codetype)
{
    return retrieve(false, codetype);
}
std::vector<unsigned int>
HeuristicCodeBloat::retrieveDecoding(const std::string &codetype,
                                     int num_baseflit, int num_codeflit)
{
    return retrieve(false, codetype, num_baseflit, num_codeflit);
}
std::vector<unsigned int>
HeuristicCodeBloat::retrieveDecoding(const std::string &codetype)
{
    return retrieve(false, codetype);
}

// hamming weight
int HeuristicCodeBloat::hammingWeight(unsigned int a)
{
    int z;
    while (a > 0) {
        ++z;
        a &= a - 1;
    }
    return z;
}
