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
// File      : decoding_state.cpp
// Created   : 22 Nov 2017 for apec_bless_noc
// Author(s) : Michael Vonbun
// E-mail(s) : michael.vonbun@tum.de
// 

#include "decoding_state.h"

// stl
#include <iostream>  // std::cout, std::endl
#include <stdexcept> // std::runtime_error

DecodingState::DecodingState(std::size_t num_bit)
    : num_bit{num_bit},
      num_fields{num_bit / BITPERFIELD + (num_bit % BITPERFIELD != 0)},
      bitvector{new field_t[num_fields]()}, current_weight{0}
{
    // num_fields = num_bits / BITPERFIELD + (num_bits % BITPERFIELD != 0);
    // bitvector = new field_t[num_fields]();
}

DecodingState::~DecodingState() { delete[] bitvector; }

field_t DecodingState::getBit(std::size_t index)
{
    if (index < num_bit) {
        field_t mask = 0x1 << (index % BITPERFIELD);
        return (bitvector[index / BITPERFIELD] & mask) >> (index % BITPERFIELD);
    } else {
        char error_msg[200];
        sprintf(error_msg, "%s:%d: Bitvector has too few bits. Requested index "
                           "%zu, max index has %zu.",
                __FILE__, __LINE__, index, num_bit - 1);
        throw std::out_of_range(error_msg);
    }
}

void DecodingState::setBit(std::size_t index, bool value)
{
    if (index < num_bit) {
        field_t bitmask = 0x1 << (index % BITPERFIELD);
        size_t array_index = index / BITPERFIELD;
        current_weight -= getWeight(bitvector[array_index]);
        if (value) {
            bitvector[array_index] = bitvector[array_index] | bitmask;
        } else {
            bitvector[array_index] = bitvector[array_index] & (~bitmask);
        }
        current_weight += getWeight(bitvector[array_index]);
    } else {
        char error_msg[200];
        sprintf(error_msg, "%s:%d: Bitvector has too few bits. Requested index "
                           "%zu, max index has %zu.",
                __FILE__, __LINE__, index, num_bit - 1);
        throw std::out_of_range(error_msg);
    }
}

field_t DecodingState::getField(std::size_t index) { return bitvector[index]; }

void DecodingState::setField(std::size_t index, field_t value)
{
    bitvector[index] = value;
}

field_t DecodingState::getFieldOfBit(std::size_t index)
{
    return bitvector[index / BITPERFIELD];
}

field_t *DecodingState::getState() { return bitvector; }

uint32_t DecodingState::getDecimalStateValue()
{
    if (num_fields < 5) {
        uint32_t dec = 0;
        for (size_t i = 0; i < num_fields; ++i) {
            dec |= (uint32_t)bitvector[i] << (BITPERFIELD * i);
        }
        return dec;
    }
}

void DecodingState::setState(field_t *new_state)
{
    if ((sizeof(new_state) / sizeof(*new_state)) == num_fields) {
        for (std::size_t i = 0; i < num_fields; ++i) {
            bitvector[i] = new_state[i];
        }
    } else {
        throw std::out_of_range(
            "Setting state failed as len(new_state) != len(state).");
    }
}

uint DecodingState::getWeight()
{
    //    uint weight = 0;
    //    for (std::size_t i = 0; i < num_fields; ++i) {
    //        weight += getWeight(bitarray[i]);
    //    }
    //    return weight;
    return current_weight;
}

uint DecodingState::getWeight(field_t a)
{
    uint z = 0;
    while (a > 0) {
        ++z;
        a &= a - 1;
    }
    return z;
}

void DecodingState::setWeight(uint weight, bool set_bitvector)
{
    if (weight <= num_bit) {
        current_weight = weight;
        if (set_bitvector) {
            size_t max_array_index = weight / BITPERFIELD;
            size_t i;
            for (i = 0; i < max_array_index; ++i) {
                bitvector[i] = FIELDMAXVAL;
            }
            // std::cout << "i is: " << i << std::endl;
            // std::cout << "weight value is: " << ((1 << (weight %
            // BITPERFIELD)) - 1) << std::endl;
            bitvector[i] = ((1 << (weight % BITPERFIELD)) - 1);
        }
    } else {
        char error_msg[200];
        sprintf(error_msg,
                "%s:%d: Bitvector has too few bits. Requested %u has %zu.",
                __FILE__, __LINE__, weight, num_bit);
        throw std::out_of_range(error_msg);
    }
}

void DecodingState::reset()
{
    for (uint i = 0; i < num_fields; ++i) {
        bitvector[i] = 0;
    }
    current_weight = 0;
}

std::size_t DecodingState::size() { return num_bit; }

std::size_t DecodingState::getNumFields() { return num_fields; }

void DecodingState::printField(std::size_t index, bool newline)
{
    // std::cout << index << ": ";
    for (uint i = BITPERFIELD; i >= 1; --i) {
        std::cout << ((bitvector[index] >> (i - 1)) & 0x1);
    }
    if (newline) {
        std::cout << std::endl;
    }
}

void DecodingState::printState()
{
    for (size_t i = num_fields; i > 0; --i) {
        // std::cout << ((int) i - 1) << ": ";
        printField(i - 1, false);
        std::cout << " ";
    }
    std::cout << std::endl;
}
