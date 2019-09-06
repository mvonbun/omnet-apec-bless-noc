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
// File      : noc_messages.cc
// Created   : 15 May 2017 for apec_bless_noc
// Author(s) : Michael Vonbun
// E-mail(s) : michael.vonbun@tum.de
// 

#include <noc_messages.h>

/**
 *
 */
Register_Class(TimeoutMsg);
TimeoutMsg::TimeoutMsg(const char *name, int kind) : TimeoutMsg_Base(name, kind)
{
}

TimeoutMsg::TimeoutMsg(const long id, const char *name, int kind)
    : TimeoutMsg(name, kind)
{
    this->packet_id = id;
}

TimeoutMsg::TimeoutMsg(const long id, ::omnetpp::simtime_t t_out,
                       const int prio, int kind, const char *name)
    : TimeoutMsg(id, name, kind)
{
    this->timeout = t_out;
    setSchedulingPriority(SP_LATE);
}

TimeoutMsg::TimeoutMsg(HeadFlit *head, const int prio, int kind,
                       const char *name)
    : TimeoutMsg(head->getPacket_id(), head->getTimeout(), prio, kind, name)
{
    //    packet_id = head->getPacket_id();
    //    timeout = head->getTimeout();
    //    setSchedulingPriority(SP_LATE);
}

TimeoutMsg::~TimeoutMsg()
{
#ifdef MSGDESTRUCTORPRINTF
    EV_DEBUG << "[FINDME] destroying timeout for packet <" << getPacket_id()
             << ">\n";
#endif
}

/**
 *
 */
Register_Class(ResourceReleaseMsg);
ResourceReleaseMsg::ResourceReleaseMsg(const char *name, int kind)
    : ResourceReleaseMsg_Base(name, kind)
{
}

ResourceReleaseMsg::ResourceReleaseMsg(int port_id, const int prio,
                                       const long message_id, const char *name,
                                       int kind)
    : ResourceReleaseMsg(name, kind)
{
    this->port_id = port_id;
    setSchedulingPriority(prio);
    this->message_id = message_id;
}

/**
 *
 */
Register_Class(PBuffOverflowMsg);
PBuffOverflowMsg::PBuffOverflowMsg(const char *name, int kind)
    : PBuffOverflowMsg_Base(name, kind)
{
}

PBuffOverflowMsg::PBuffOverflowMsg(int module_id, const char *name, int kind)
    : PBuffOverflowMsg(name, kind)
{
    this->module_id = module_id;
}

/**
 *
 */
Register_Class(ConvergenceMsg);
ConvergenceMsg::ConvergenceMsg(const char *name, int kind)
    : ConvergenceMsg_Base(name, kind)
{
}

ConvergenceMsg::ConvergenceMsg(int module_id, ::omnetpp::simtime_t avg_latency,
                               const char *name, int kind)
    : ConvergenceMsg(name, kind)
{
    this->avg_latency = avg_latency;
}
