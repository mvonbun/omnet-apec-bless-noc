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
// File      : stl_fifo.h
// Created   : 05 Jul 2017 for apec_bless_noc
// Author(s) : Michael Vonbun
// E-mail(s) : michael.vonbun@tum.de
// 

#ifndef CONTAINERS_STL_FIFO_H_
#define CONTAINERS_STL_FIFO_H_

#include <queue>

/** Finite size FIFO using STL containers.
 *
 *  If size is 0, the FIFO has no size limitation.
 */
template <typename T> class Fifo
{
public:
    /// Initialize infinite size Fifo.
    Fifo() : size_(0){};

    /// Intitialize finite @param size Fifo.
    Fifo(std::size_t size) : size_(size){};

    virtual ~Fifo(){};

    /// Resize fifo size.
    void resize(std::size_t size) { size_ = size; };

    /// Push @param element to Fifo.
    /// If the Fifo is full already, the oldest element is discarded.
    void push(T element)
    {
        if ((size_ == 0) || (buffer_.size() <= size_)) {
            buffer_.push(element);
        } else {
            pop();
            push(element);
        }
    };

    /// Pop oldest element.
    void pop() { buffer_.pop(); };

    /// Access oldest element (top of Fifo).
    T top() { return buffer_.front(); };

    /// @return true if Fifo is empty.
    bool empty() { return buffer_.empty(); };

    /// @return true if Fifo reached its size limit.
    bool full() { return ((size_ != 0) && (buffer_.size() == size_)); };

private:
    std::size_t size_;     ///< Fifo size limit.
    std::queue<T> buffer_; ///< Fifo buffer.
};

#endif /* CONTAINERS_STL_FIFO_H_ */
