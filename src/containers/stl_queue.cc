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
// File      : stl_queue.cc
// Created   : 22 Mai 2017 for apec_bless_noc
// Author(s) : Michael Vonbun
// E-mail(s) : michael.vonbun@tum.de
// 

#include <containers/stl_queue.h>

STLQueue::~STLQueue() { clear(this->Q); }

void STLQueue::clear(queue_t &q)
{
    for (queue_t::iterator it = q.begin(); it != q.end(); ++it) {
        delete (*it);
    }
    q.clear();
}

void STLQueue::push(msg_t *msg)
{
    // TODO: take(msg) maybe
    this->Q.push_back(msg);
}

msg_t *STLQueue::pop()
{
    // TODO: drop(msg) maybe
    msg_t *msg = nullptr;
    if (Q.size() > 0) {
        msg = Q.front();
        this->Q.pop_front();
    }
    return msg;
}

size_t STLQueue::size() { return this->Q.size(); }

int STLQueue::find_packet(const int id, queue_t &q, queue_t::iterator &iter,
                          const bool return_len)
{
    EV_DEBUG << "Searching for packet<" << id << "> in STL Queue: ";
    int idx = 0;
    int num_flit = 0;
    head_t *head;
    for (queue_t::iterator it = q.begin(); it != q.end(); ++it, ++idx) {
        // check headers for id
        if ((*it)->getKind() == HEAD) {
            head = (head_t *)(*it);
            if (head->getPacket_id() == id) {
                num_flit = head->getNum_flit();
                iter = it;
                break;
            }
        }
    }

    if (num_flit > 0) {
        EV_DEBUG << "found at <" << idx << ">\n";
    } else {
        idx = -1;
        EV_DEBUG << "not found\n";
    }

    if (return_len) {
        return num_flit;
    } else {
        return idx;
    }
}

bool STLQueue::find_packet(queue_t &q, const int id, int &idx,
                           queue_t::iterator &iter)
{
    EV_DEBUG << "Searching for packet<" << id << "> in STL Queue: ";
    idx = 0;
    bool packet_found = false;
    for (queue_t::iterator it = q.begin(); it != q.end(); ++it, ++idx) {
        // find start of packet
        if ((*it)->getKind() == HEAD) {
            if (((head_t *)(*it))->getPacket_id() == id) {
                iter = it;
                packet_found = true;
                break;
            }
        }
    }
    (packet_found) ? (EV_DEBUG << "found at <" << idx << ">\n")
                   : (EV_DEBUG << "not found\n");
    return packet_found;
}

int STLQueue::remove_packet(queue_t &q, const int id)
{
    EV_DEBUG << "deleting Packet <" << id << ">";
    EV_DEBUG << " from STL Queue";
    EV_DEBUG << "\n";

    int idx(-1);
    queue_t::iterator iter;
    int num_flit(0);

    // as find sets the iterator to the match, we can use this to start looping
    if (find_packet(q, id, idx, iter)) {
        num_flit = ((head_t *)(*iter))->getNum_flit();

        head_t *flit;
        bool erase = true;
        for (int i = 0; i < num_flit; ++i) {
            if (erase) {
                if ((*iter)->getKind() == TAIL) {
                    erase = false;
                }
                flit = (head_t *)(*iter);
                EV_DEBUG << "try to delete flit " << flit->getPacket_id() << ":"
                         << flit->getSequence_number() << "\n";
                delete *iter;
                iter = q.erase(iter);
            }
        }
    }

    EV_DEBUG << "deleted " << num_flit << " flits\n";
    return num_flit;
}
