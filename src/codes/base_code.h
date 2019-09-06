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
// File      : base_code.h
// Created   : 03 Apr 2017 for apec_bless_noc
// Author(s): Michael Vonbun
// E-mail(s): michael.vonbun@tum.de
// 

#ifndef __BASE_CODE_H_
#define __BASE_CODE_H_

//
// base code class
//
// The received symbols are kept in a bitvector by the caller.
//
// The class provides methods to
// - set the i-th bit of a bitvector
// - indicate if the bitvector is decodable (i.e. if the sequence
//   received so far is decodable)
//
class BaseCode
{
public:
    virtual bool decodable(int num_baseflit, unsigned int received) = 0;
    void received(int i, unsigned int &received);

protected:
    int hammingWeight(unsigned int received);
};

#endif
