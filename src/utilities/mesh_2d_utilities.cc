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
// File      : mesh_2d_uilities.cc
// Created   : 03 Jul 2017 for apec_bless_noc
// Author(s) : Michael Vonbun
// E-mail(s) : michael.vonbun@tum.de
// 

#include <mesh_2d_utilities.h>

int util::getRow(const int id, const int num_row, const int num_col,
                 const int offset, const bool transpose)
{
    if (transpose) {
        return (id + offset) / num_row;
    } else {
        return (id + offset) / num_col;
    }
}

int util::getColumn(const int id, const int num_row, const int num_col,
                    const int offset, const bool transpose)
{
    if (transpose) {
        return (id + offset) % num_row;
    } else {
        return (id + offset) % num_col;
    }
}
