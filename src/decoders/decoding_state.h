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
// File      : decoding_state.h
// Created   : 22 Nov 2017 for apec_bless_noc
// Author(s) : Michael Vonbun
// E-mail(s) : michael.vonbun@tum.de
// 

#ifndef DECODERS_DECODINGSTATUS_H_
#define DECODERS_DECODINGSTATUS_H_

// stl
#include <cstddef> // std::size_t
#include <cstdint> // typedefs
#include <map>     // std::map

typedef uint8_t field_t;
#define BITPERFIELD 8
#define FIELDMAXVAL 255

/** Bitvector based decoding state class.
 *
 * The size of the decoding state is set in the constructor.
 *
 */
class DecodingState
{
public:
    DecodingState(std::size_t num_bit = 1024);
    virtual ~DecodingState();

    /// @return bit at position @param index.
    field_t getBit(std::size_t index);

    /// Set bit at position @param index to @param value.
    void setBit(std::size_t index, bool value = true);

    /// @return decimal value of @param index-th slice of bitvector.
    field_t getField(std::size_t index);

    /// Set field at @param index to @param value.
    void setField(std::size_t index, field_t value = FIELDMAXVAL);

    /// @return field of @param index of a bit.
    field_t getFieldOfBit(std::size_t index);

    /// @return bitvector array.
    field_t *getState();

    /// @return decimal state value if smaller then 2^32-1
    uint32_t getDecimalStateValue();

    // char* getHexStateValue();

    /// Set bitvector array.
    void setState(field_t *new_state);

    /// @return the hamming weight of the entire bitvector.
    uint getWeight();

    /// Set weight to
    void setWeight(uint weight, bool set_bitvector = false);

    /// Reset the decoding state.
    void reset();

    /// @return number of bits of bitvector
    std::size_t size();

    /// @return number of fields
    std::size_t getNumFields();

    /// Print the 8 bit sequence of the @param index-th slice of bitvector to
    /// std::cout. Terminate the output with a newline if @param newline is
    /// true.
    void printField(std::size_t index, bool newline = true);

    /// Print entire bitvector. Left is the MSB, right the LSB.
    void printState();

protected:
    std::size_t num_bit;    //< Number of bits of bitvector.
    std::size_t num_fields; //< Number of array fields.
    field_t *bitvector;     //< Bitvector array holding the state.
    uint current_weight;    //< Store the current weight.

    /// @return the hamming weight of a single field of the bitvector.
    uint getWeight(field_t a);
};

#endif /* DECODERS_DECODINGSTATUS_H_ */
