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

#ifndef __HEURISTIC_CODE_BLOAT_H_
#define __HEURISTIC_CODE_BLOAT_H_

#include <cassert>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

class HeuristicCodeBloat
{
private:
    int m_num_baseflit;
    int m_num_codeflit;
    std::string m_codetype;

    std::vector<unsigned int> m_codewords;
    std::vector<unsigned int> m_syndrome;

public:
    // constructors
    HeuristicCodeBloat(int num_baseflit, int num_codeflit)
    {
        m_codewords = retrieveEncoding("burst", num_baseflit, num_codeflit);
        m_syndrome = retrieveDecoding("burst", num_baseflit, num_codeflit);
    }
    HeuristicCodeBloat(const std::string &codetype, int num_baseflit,
                       int num_codeflit)
    {
        m_codewords = retrieveEncoding(codetype, num_baseflit, num_codeflit);
        m_syndrome = retrieveDecoding(codetype, num_baseflit, num_codeflit);
    }
    HeuristicCodeBloat(const char *codetype, int num_baseflit, int num_codeflit)
    {
        m_codewords = retrieveEncoding(codetype, num_baseflit, num_codeflit);
        m_syndrome = retrieveDecoding(codetype, num_baseflit, num_codeflit);
    }

    // check received packet
    bool decodable(unsigned int received);

    // access codebook
    std::vector<unsigned int> getCode();
    unsigned int getCodeWord(size_t i);
    size_t codeSize();

    // access syndrome
    std::vector<unsigned int> getSyndrome();
    unsigned int getSyndromeWord(size_t i);
    size_t syndromeSize();

    //
    std::vector<unsigned int> getFlitsInCodeWord(size_t k);

    // some user information printing
    void printCodeInfo();
    void printGeneratorMatrix();

private:
    void setCodeProperties(int num_baseflit, int num_codeflit);

    std::vector<unsigned int> readFromFile(bool enc);

    std::vector<unsigned int> retrieve(bool enc, const char *codetype,
                                       int num_baseflit, int num_codeflit);
    std::vector<unsigned int> retrieve(bool enc, const char *codetype);
    std::vector<unsigned int> retrieve(bool enc, const std::string &codetype,
                                       int num_baseflit, int num_codeflit);
    std::vector<unsigned int> retrieve(bool enc, const std::string &codetype);

    std::vector<unsigned int>
    retrieveEncoding(const char *codetype, int num_baseflit, int num_codeflit);
    std::vector<unsigned int> retrieveEncoding(const char *codetype);
    std::vector<unsigned int> retrieveEncoding(const std::string &codetype,
                                               int num_baseflit,
                                               int num_codeflit);
    std::vector<unsigned int> retrieveEncoding(const std::string &codetype);

    std::vector<unsigned int>
    retrieveDecoding(const char *codetype, int num_baseflit, int num_codeflit);
    std::vector<unsigned int> retrieveDecoding(const char *codetype);
    std::vector<unsigned int> retrieveDecoding(const std::string &codetype,
                                               int num_baseflit,
                                               int num_codeflit);
    std::vector<unsigned int> retrieveDecoding(const std::string &codetype);

    int hammingWeight(unsigned int a);
};

#endif
