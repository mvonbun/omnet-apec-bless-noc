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
// File      : noc_flits.cc
// Created   : 02 May 2017 for apec_bless_noc
// Author(s) : Michael Vonbun
// E-mail(s) : michael.vonbun@tum.de
// 

#include <debug_messages.h>
#include <noc_flits.h>

/** Header flit.
 *
 */
Register_Class(HeadFlit);
HeadFlit::HeadFlit(const char *name, int kind) : HeadFlit_Base(name, kind)
{
    this->source = -1;
    this->destination = -1;
    this->num_codeflit = 0;
    this->attempts = 0;
}

HeadFlit::HeadFlit(long _id, int _kind, const char *_name)
    : HeadFlit(_name, _kind)
{
    this->packet_id = _id;
}

HeadFlit::HeadFlit(long _id, int _num_baseflit, int _priority,
                   ::omnetpp::simtime_t _gen_time, int _kind, const char *_name)
    : HeadFlit(_id, _kind, _name)
{
    this->generation_time = _gen_time;
    this->priority = _priority;
    this->num_flit = _num_baseflit;
    this->num_baseflit = _num_baseflit;
}

HeadFlit::~HeadFlit()
{
#ifdef MSGDESTRUCTORPRINTF
    EV_DEBUG << "[DELETE] msg<" << this->getId() << "> :: head of "
             << PACKET(this) << "\n";
#endif
}

void HeadFlit::setNum_flit()
{
    this->num_flit = this->num_codeflit + this->num_baseflit;
}

void HeadFlit::setNum_baseflit(int num_baseflit)
{
    this->num_baseflit = num_baseflit;
    this->setNum_flit();
}

void HeadFlit::setNum_codeflit(int num_codeflit)
{
    this->num_codeflit = num_codeflit;
    this->setNum_flit();
}

void HeadFlit::resetAttempts() { this->attempts = 0; }

void HeadFlit::newAttempt() { ++this->attempts; }

::omnetpp::simtime_t HeadFlit::getTimeout() const
{
    // regular packet timeout
    if (repetition_timeout.empty()) {
        return this->timeout;
    } else {
        // repetition coded packet
        // after last repetition is out, use regular timeout
        EV_DEBUG << "Requesting timeout <" << this->attempts << ">\n";
        int t_out_index = (this->attempts + 1) % this->num_flit;
        if (t_out_index == 0) {
            EV_DEBUG << "Return regular timeout\n";
            return this->timeout;
        } else {
            EV_DEBUG << "Return repetition timeout\n";
            return repetition_timeout[(t_out_index %
                                       repetition_timeout.size())];
        }
    }
}

void HeadFlit::addTimeout(::omnetpp::simtime_t t_add)
{
    this->timeout += t_add;
}

void HeadFlit::addRepetitionInterval(::omnetpp::simtime_t t_out)
{
    repetition_timeout.push_back(t_out);
}

bool HeadFlit::hasMutlipleTimeouts() { return !repetition_timeout.empty(); }

void HeadFlit::copy(const HeadFlit &other) {}

/** Body flits.
 *
 */
Register_Class(BodyFlit);
BodyFlit::BodyFlit(const char *name, int kind) : BodyFlit_Base(name, kind) {}

BodyFlit::BodyFlit(int sequence_number, int kind, const char *name)
    : BodyFlit_Base(name, kind)
{
    this->sequence_number = sequence_number;
}

BodyFlit::~BodyFlit()
{
#ifdef MSGDESTRUCTORPRINTF
    EV_DEBUG << "[DELETE] msg<" << this->getId() << "> :: body <"
             << sequence_number << ">\n";
#endif
}

void BodyFlit::copy(const BodyFlit &other) {}

/** Tail flit.
 *
 */
Register_Class(TailFlit);
TailFlit::TailFlit(const char *name, int kind) : TailFlit_Base(name, kind) {}

TailFlit::TailFlit(int sequence_number, int kind, const char *name)
    : TailFlit(name, kind)
{
    this->sequence_number = sequence_number;
}

TailFlit::~TailFlit()
{
#ifdef MSGDESTRUCTORPRINTF
    EV_DEBUG << "[DELETE] msg<" << this->getId() << "> :: tail <"
             << sequence_number << ">\n";
#endif
}

void TailFlit::copy(const TailFlit &other) {}

/** ACK flit.
 *
 */
Register_Class(AckFlit);
AckFlit::AckFlit(const char *name, int kind) : AckFlit_Base(name, kind)
{
    this->generation_time = ::omnetpp::simTime();
}

AckFlit::AckFlit(const HeadFlit *head, int kind, const char *name)
    : AckFlit(name, kind)
{
    // to send back:
    //  reverse source and destination
    //  include id of packet the ACK corresponds to
    this->source = head->getDestination();
    this->destination = head->getSource();
    this->packet_id = head->getPacket_id();
    this->packet_attempts = head->getAttempts();
    this->packet_priority = head->getPriority();
    this->packet_num_flit = head->getNum_flit();
    this->packet_injection_time = head->getInjection_time();
    this->packet_generation_time = head->getGeneration_time();
}

AckFlit::~AckFlit()
{
#ifdef MSGDESTRUCTORPRINTF
    EV_DEBUG << "[DELETE] msg<" << this->getId() << "> :: ack for packet<"
             << getPacket_id() << ">";
#ifndef VALIDATE
    EV_DEBUG << " t_gen=" << this->getCreationTime();
    if (this->getOwner()) {
        EV_DEBUG << " owner<" << this->getOwner()->getFullPath() << ">";
    }
    if (this->getArrivalModule()) {
        EV_DEBUG << " arrived<" << this->getArrivalModule()->getFullName()
                 << ">";
    }
    if (this->getSenderModule()) {
        EV_DEBUG << " sender<" << this->getSenderModule()->getFullName() << ">";
    }
#endif
    EV_DEBUG << "\n";
#endif
}

void AckFlit::copy(const AckFlit &other) {}

/** NACK flit.
 *
 */
Register_Class(NackFlit);
NackFlit::NackFlit(const char *name, int kind) : NackFlit_Base(name, kind)
{
    this->generation_time = ::omnetpp::simTime();
    this->carries_decoder_info = false;
}

NackFlit::NackFlit(const HeadFlit *head, int kind, const char *name)
    : NackFlit(name, kind)
{
    // to send back:
    //  reverse source and destination
    //  include id of packet the ACK corresponds to
    this->source = head->getDestination();
    this->destination = head->getSource();
    this->packet_id = head->getPacket_id();
    this->packet_attempts = head->getAttempts();
    this->packet_priority = head->getPriority();
    this->packet_num_flit = head->getNum_flit();
    this->packet_injection_time = head->getInjection_time();
    this->packet_generation_time = head->getGeneration_time();
}

NackFlit::~NackFlit()
{
#ifdef MSGDESTRUCTORPRINTF
    EV_DEBUG << "[DELETE] msg<" << this->getId() << "> :: nack for packet<"
             << getPacket_id() << ">";
#ifndef VALIDATE
    EV_DEBUG << " t_gen=" << this->getCreationTime();
    if (this->getOwner()) {
        EV_DEBUG << " owner<" << this->getOwner()->getFullPath() << ">";
    }
    if (this->getArrivalModule()) {
        EV_DEBUG << " arrived<" << this->getArrivalModule()->getFullName()
                 << ">";
    }
    if (this->getSenderModule()) {
        EV_DEBUG << " sender<" << this->getSenderModule()->getFullName() << ">";
    }
#endif
    EV_DEBUG << "\n";
#endif
}

void NackFlit::addMissingFlit(int missing_idx)
{
    carries_decoder_info = true;
    missing_flits.push_back(missing_idx);
}

bool NackFlit::carriesDecoderInfo() { return this->carries_decoder_info; }

void NackFlit::copy(const NackFlit &other) {}

/** cNACK flit.
 *
 */
Register_Class(CNackFlit);
CNackFlit::CNackFlit(const char *name, int kind) : CNackFlit_Base(name, kind) {}

std::vector<NackFlit *> CNackFlit::getNackVector() { return this->nacks; }

void CNackFlit::addNum_nacks_combined(int num) { num_nacks_combined += num; }

void CNackFlit::pushNack(NackFlit *nack)
{
    nacks.push_back(nack);
    ++num_nacks_combined;
}

void CNackFlit::pushNack(CNackFlit *cnack)
{
    int num_nacks_to_insert = cnack->getNum_nacks_combined();
    num_nacks_combined += num_nacks_to_insert;
    for (int i = 0; i < num_nacks_to_insert; ++i) {
        nacks.push_back(cnack->popNack());
    }
}

NackFlit *CNackFlit::popNack()
{
    if (!nacks.empty()) {
        NackFlit *tmp = nacks.back();
        nacks.pop_back();
        num_nacks_combined--;
        return tmp;
    } else {
        return nullptr;
    }
}

void CNackFlit::copy(const CNackFlit &other) {}
