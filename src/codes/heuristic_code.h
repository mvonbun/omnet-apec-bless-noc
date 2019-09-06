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
// File      : heuristic_code.h
// Created   : 03 Apr 2017 for apec_bless_noc
// Author(s) : Michael Vonbun
// E-mail(s) : michael.vonbun@tum.de
// 

#ifndef __HEURISTIC_CODE_H_
#define __HEURISTIC_CODE_H_

#include <string>
#include <vector>

#include "base_code.h"

//
// heuristic codes are different, as there is one distinct code per
// (num_baseflit, num_codeflit) tuple
//
class HeuristicCode : public BaseCode
{
protected:
    int m_num_baseflit;
    int m_num_codeflit;
    std::string m_codetype;
    std::vector<unsigned int> m_syndrome;

public:
    HeuristicCode(const std::string &codetype, int num_baseflit,
                  int num_codeflit)
        : m_num_baseflit(num_baseflit), m_num_codeflit(num_codeflit),
          m_codetype(codetype), m_syndrome(readIntoMemberFromFile()){};
    HeuristicCode(const char *codetype, int num_baseflit, int num_codeflit)
        : m_num_baseflit(num_baseflit), m_num_codeflit(num_codeflit),
          m_codetype(codetype), m_syndrome(readIntoMemberFromFile()){};

    bool decodable(const int num_baseflit, const unsigned int received);
    bool decodable(unsigned int received);

    size_t size() { return m_syndrome.size(); };

protected:
    std::vector<unsigned int> readIntoMemberFromFile();
};

// code robust against single (independent) intersecters
class HeuristicSingleCode : public HeuristicCode
{
public:
    HeuristicSingleCode(int num_baseflit, int num_codeflit)
        : HeuristicCode("single", num_baseflit, num_codeflit){};
};

// code robust against bursts
class HeuristicBurstCode : public HeuristicCode
{
public:
    HeuristicBurstCode(int num_baseflit, int num_codeflit)
        : HeuristicCode("burst", num_baseflit, num_codeflit){};
};

// code robust against both single and bursty intersecters
class HeuristicJointCode : public HeuristicCode
{
public:
    HeuristicJointCode(int num_baseflit, int num_codeflit)
        : HeuristicCode("joint", num_baseflit, num_codeflit){};
};

#endif
