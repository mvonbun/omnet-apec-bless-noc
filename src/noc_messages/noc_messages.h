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
// File      : noc_messages.h
// Created   : 15 May 2017 for apec_bless_noc
// Author(s) : Michael Vonbun
// E-mail(s) : michael.vonbun@tum.de
// 

#ifndef NOC_MESSAGES_NOC_MESSAGES_H_
#define NOC_MESSAGES_NOC_MESSAGES_H_

#include <noc_flits.h>
#include <noc_messages_m.h>

class TimeoutMsg : public TimeoutMsg_Base
{
private:
    void copy(const TimeoutMsg &other) {}
public:
    TimeoutMsg(const char *name = nullptr, int kind = TIMEOUT);
    TimeoutMsg(const long id, const char *name = nullptr, int kind = TIMEOUT);
    TimeoutMsg(const long id, ::omnetpp::simtime_t t_out,
               const int prio = SP_BASE, int kind = TIMEOUT,
               const char *name = nullptr);
    TimeoutMsg(HeadFlit *head, const int prio = SP_BASE, int kind = TIMEOUT,
               const char *name = nullptr);

    virtual ~TimeoutMsg();

    TimeoutMsg(const TimeoutMsg &other) : TimeoutMsg_Base(other)
    {
        copy(other);
    }
    TimeoutMsg &operator=(const TimeoutMsg &other)
    {
        if (this == &other)
            return *this;
        TimeoutMsg_Base::operator=(other);
        copy(other);
        return *this;
    }
    virtual TimeoutMsg *dup() const { return new TimeoutMsg(*this); }

    // ADD CODE HERE to redefine and implement pure virtual functions from
    // TimeoutMsg_Base
};

class ResourceReleaseMsg : public ResourceReleaseMsg_Base
{
private:
    void copy(const ResourceReleaseMsg &other) {}

public:
    ResourceReleaseMsg(const char *name = nullptr, int kind = RELEVENT);
    ResourceReleaseMsg(int port_id, const int prio, const long message_id = -1,
                       const char *name = nullptr, int kind = RELEVENT);

    ResourceReleaseMsg(const ResourceReleaseMsg &other)
        : ResourceReleaseMsg_Base(other)
    {
        copy(other);
    }
    ResourceReleaseMsg &operator=(const ResourceReleaseMsg &other)
    {
        if (this == &other)
            return *this;
        ResourceReleaseMsg_Base::operator=(other);
        copy(other);
        return *this;
    }
    virtual ResourceReleaseMsg *dup() const
    {
        return new ResourceReleaseMsg(*this);
    }
    // ADD CODE HERE to redefine and implement pure virtual functions from
    // ResourceReleaseMsg_Base
};

class PBuffOverflowMsg : public PBuffOverflowMsg_Base
{
private:
    void copy(const PBuffOverflowMsg &other) {}

public:
    PBuffOverflowMsg(const char *name = nullptr, int kind = CBUFOVFL);
    PBuffOverflowMsg(int module_id, const char *name = nullptr,
                     int kind = CBUFOVFL);

    PBuffOverflowMsg(const PBuffOverflowMsg &other)
        : PBuffOverflowMsg_Base(other)
    {
        copy(other);
    }
    PBuffOverflowMsg &operator=(const PBuffOverflowMsg &other)
    {
        if (this == &other)
            return *this;
        PBuffOverflowMsg_Base::operator=(other);
        copy(other);
        return *this;
    }
    virtual PBuffOverflowMsg *dup() const
    {
        return new PBuffOverflowMsg(*this);
    }
    // ADD CODE HERE to redefine and implement pure virtual functions from
    // PBuffOverflowMsg_Base
};

class ConvergenceMsg : public ConvergenceMsg_Base
{
private:
    void copy(const ConvergenceMsg &other) {}

public:
    ConvergenceMsg(const char *name = nullptr, int kind = CCONV);
    ConvergenceMsg(int module_id, ::omnetpp::simtime_t avg_latency,
                   const char *name = nullptr, int kind = CCONV);

    ConvergenceMsg(const ConvergenceMsg &other) : ConvergenceMsg_Base(other)
    {
        copy(other);
    }
    ConvergenceMsg &operator=(const ConvergenceMsg &other)
    {
        if (this == &other)
            return *this;
        ConvergenceMsg_Base::operator=(other);
        copy(other);
        return *this;
    }
    virtual ConvergenceMsg *dup() const { return new ConvergenceMsg(*this); }
    // ADD CODE HERE to redefine and implement pure virtual functions from
    // ConvergenceMsg_Base
};

#endif /* NOC_MESSAGES_NOC_MESSAGES_H_ */
