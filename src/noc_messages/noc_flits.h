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
// File      : noc_flits.h
// Created   : 02 Mai 2017 for apec_bless_noc
// Author(s) : Michael Vonbun
// E-mail(s) : michael.vonbun@tum.de
// 

#ifndef NOC_MESSAGES_NOC_FLITS_H_
#define NOC_MESSAGES_NOC_FLITS_H_

// stl
#include <deque>  // std::deque
#include <vector> // std::vector

// base message file
#include <noc_flits_m.h>

/** */
class HeadFlit : public HeadFlit_Base
{
private:
    void copy(const HeadFlit &other);

public:
    HeadFlit(const char *name = nullptr, int kind = HEAD);
    HeadFlit(long _id, int _kind = HEAD, const char *_name = nullptr);
    HeadFlit(long _id, int _baseflit, int _priority,
             ::omnetpp::simtime_t _gen_time, int _kind = HEAD,
             const char *_name = nullptr);

    virtual ~HeadFlit();

    HeadFlit(const HeadFlit &other) : HeadFlit_Base(other) { copy(other); }
    HeadFlit &operator=(const HeadFlit &other)
    {
        if (this == &other)
            return *this;
        HeadFlit_Base::operator=(other);
        copy(other);
        return *this;
    }
    virtual HeadFlit *dup() const { return new HeadFlit(*this); }

    virtual void setNum_baseflit(int num_baseflit);
    virtual void setNum_codeflit(int num_codeflit);
    void resetAttempts();
    void newAttempt();
    virtual ::omnetpp::simtime_t getTimeout() const;
    virtual void addTimeout(::omnetpp::simtime_t t_add);
    virtual void addRepetitionInterval(::omnetpp::simtime_t timeout);
    virtual bool hasMutlipleTimeouts();

private:
    using HeadFlit_Base::setNum_flit;  // change method property
    using HeadFlit_Base::setAttempts;  // change method property
    using HeadFlit_Base::setPacket_id; // change method property
    void setNum_flit();
    std::deque<::omnetpp::simtime_t> repetition_timeout;
};

/** */
class BodyFlit : public BodyFlit_Base
{
private:
    void copy(const BodyFlit &other);

public:
    BodyFlit(const char *name = nullptr, int kind = 0);
    BodyFlit(int sequence_number, int kind = 0, const char *name = nullptr);

    virtual ~BodyFlit();

    BodyFlit(const BodyFlit &other) : BodyFlit_Base(other) { copy(other); }
    BodyFlit &operator=(const BodyFlit &other)
    {
        if (this == &other)
            return *this;
        BodyFlit_Base::operator=(other);
        copy(other);
        return *this;
    }
    virtual BodyFlit *dup() const { return new BodyFlit(*this); }
};

/** */
class TailFlit : public TailFlit_Base
{
private:
    void copy(const TailFlit &other);

public:
    TailFlit(const char *name = nullptr, int kind = 0);
    TailFlit(int sequence_number, int kind = 0, const char *name = nullptr);

    virtual ~TailFlit();

    TailFlit(const TailFlit &other) : TailFlit_Base(other) { copy(other); }
    TailFlit &operator=(const TailFlit &other)
    {
        if (this == &other)
            return *this;
        TailFlit_Base::operator=(other);
        copy(other);
        return *this;
    }
    virtual TailFlit *dup() const { return new TailFlit(*this); }
};

/** */
class AckFlit : public AckFlit_Base
{
private:
    void copy(const AckFlit &other);

public:
    AckFlit(const char *name = nullptr, int kind = ACK);
    AckFlit(const HeadFlit *head, int kind = ACK, const char *name = nullptr);

    virtual ~AckFlit();

    AckFlit(const AckFlit &other) : AckFlit_Base(other) { copy(other); }
    AckFlit &operator=(const AckFlit &other)
    {
        if (this == &other)
            return *this;
        AckFlit_Base::operator=(other);
        copy(other);
        return *this;
    }
    virtual AckFlit *dup() const { return new AckFlit(*this); }
};

/** */
class NackFlit : public NackFlit_Base
{
private:
    void copy(const NackFlit &other);

public:
    NackFlit(const char *name = nullptr, int kind = NACK);
    NackFlit(const HeadFlit *head, int kind = NACK, const char *name = nullptr);

    virtual ~NackFlit();

    void addMissingFlit(int missing_idx);
    bool carriesDecoderInfo();

    NackFlit(const NackFlit &other) : NackFlit_Base(other) { copy(other); }
    NackFlit &operator=(const NackFlit &other)
    {
        if (this == &other)
            return *this;
        NackFlit_Base::operator=(other);
        copy(other);
        return *this;
    }
    virtual NackFlit *dup() const { return new NackFlit(*this); }
};

/** */
class CNackFlit : public CNackFlit_Base
{
private:
    void copy(const CNackFlit &other);

public:
    CNackFlit(const char *name = nullptr, int kind = CNACK);

    CNackFlit(const CNackFlit &other) : CNackFlit_Base(other) { copy(other); }
    CNackFlit &operator=(const CNackFlit &other)
    {
        if (this == &other)
            return *this;
        CNackFlit_Base::operator=(other);
        copy(other);
        return *this;
    }
    virtual CNackFlit *dup() const { return new CNackFlit(*this); }

    // ADD CODE HERE to redefine and implement pure virtual functions from
    // CNackFlit_Base
private:
    std::vector<NackFlit *> nacks;

public:
    std::vector<NackFlit *> getNackVector();
    void addNum_nacks_combined(int num);
    void pushNack(NackFlit *nack);
    void pushNack(CNackFlit *cnack);
    NackFlit *popNack();
};

#endif /* NOC_MESSAGES_NOC_FLITS_H_ */
