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
// File      : stl_queue.h
// Created   : 22 Mai 2017 for apec_bless_noc
// Author(s) : Michael Vonbun
// E-mail(s) : michael.vonbun@tum.de
// 

#ifndef __APEC_BLESS_NOC_FIFO_H_
#define __APEC_BLESS_NOC_FIFO_H_

#include <deque>
#include <list>
#include <noc_flits.h>
#include <omnetpp.h>

using namespace omnetpp;

typedef std::list<RootFlit *> queue_t;
typedef RootFlit msg_t;
typedef HeadFlit head_t;

/** Custom Queue class using STL containers.
 *
 *  Acts as an extended FIFO, that allows to delete packets within the FIFO.
 *
 */
class STLQueue
{
private:
    queue_t Q;
    queue_t::iterator m_it;
    bool find_packet(queue_t &q, const int id, int &idx,
                     queue_t::iterator &iter);
    int find_packet(const int id) { return find_packet(id, this->Q); };
    int find_packet(const int id, queue_t &q)
    {
        return find_packet(id, q, this->m_it);
    };

    /// Find packet in queue by its id.
    ///
    /// @param[in]  id          packet id to look for
    /// @param[in]  q           queue to look in
    /// @param[out] iter        iterator to the match (q.end() if no match was
    /// found)
    /// @param[in]  return_len  control return type
    /// @return     return lengthor queue position of found package
    ///
    int find_packet(const int id, queue_t &q, queue_t::iterator &iter,
                    const bool return_len = false);

    void clear(queue_t &q);

public:
    STLQueue() : Q(), m_it(){};
    virtual ~STLQueue();

    void push(msg_t *msg);
    msg_t *pop();
    size_t size();
    int remove_packet(queue_t &q, const int id);
    bool has_packet(const int id);
};

#endif
