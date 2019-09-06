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
// File      : enum_utilities.cc
// Created   : 02 Mai 2017 for apec_bless_noc
// Author(s) : Michael Vonbun
// E-mail(s) : michael.vonbun@tum.de
// 

#include <enum_definitions_m.h>
#include <enum_utilities.h>
#include <stdexcept>

namespace util
{

int convertMessageType(const std::string &str)
{
    if (str == "EVENT")
        return EVENT;
    else if (str == "ACK")
        return ACK;
    else if (str == "HEAD")
        return HEAD;
    else if (str == "DATA")
        return DATA;
    else if (str == "BODY")
        return BODY;
    else if (str == "TAIL")
        return TAIL;
    else
        throw std::invalid_argument("Message Type");
}

int convertCodeType(const std::string &str)
{
    if (str == "UNCODED")
        return UNCODED;
    else if (str == "CYCLIC")
        return CYCLIC;
    else if (str == "DYNAMIC")
        return DYNAMIC;
    else if (str == "HEUR")
        return HEUR;
    else if (str == "HEUR_S")
        return HEUR_S;
    else if (str == "HEUR_B")
        return HEUR_B;
    else if (str == "HEUR_J")
        return HEUR_J;
    else if (str == "FEC")
        return FEC;
    else
        throw std::invalid_argument("Code Type");
}

int convertResourceReleaseType(const std::string &str)
{
    if (str == "TAILFLAG" || str == "Flag")
        return TAILFLAG;
    else if (str == "FLITCOUNT" || str == "Length")
        return FLITCOUNT;
    else
        throw std::invalid_argument("Resource Release Type");
}

int convertAckType(const std::string &str)
{
    if (str == "ACK" || str == "Ack")
        return ACK;
    else if (str == "NACK" || str == "Nack")
        return NACK;
    else
        throw std::invalid_argument("Acknowledgment Type");
}

std::string convertMessageType(const int type)
{
    switch (type) {
    case EVENT:
        return "EVENT";
    case TXEVENT:
        return "TXEVENT";
    case TIMEOUT:
        return "TIMEOUT";
    case ARBEVENT:
        return "ARBEVENT";
    case RELEVENT:
        return "RELEVENT";
    case FLIT:
        return "FLIT";
    case HEAD:
        return "HEAD";
    case HEADTAIL:
        return "HEADTAIL";
    case ACK:
        return "ACK";
    case DATA:
        return "DATA";
    case BODY:
        return "BODY";
    case TAIL:
        return "TAIL";
    default:
        throw std::invalid_argument("Message Type");
    }
}

std::string convertCodeType(const int type)
{
    switch (type) {
    case UNCODED:
        return "UNCODED";
    case CYCLIC:
        return "CYCLIC";
    case DYNAMIC:
        return "DYNAMIC";
    case HEUR:
        return "HEUR";
    case HEUR_S:
        return "HEUR_S";
    case HEUR_B:
        return "HEUR_B";
    case HEUR_J:
        return "HEUR_J";
    case FEC:
        return "FEC";
    default:
        throw std::invalid_argument("Code Type");
    }
}

std::string convertResourceReleaseType(const int type)
{
    if (type == TAILFLAG)
        return "TAILFLAG";
    else if (type == FLITCOUNT)
        return "FLITCOUNT";
    else
        throw std::invalid_argument("Resource Release Type");
}

std::string convertAckType(const int type)
{
    if (type == ACK)
        return "ACK";
    else if (type == NACK)
        return "NACK";
    else
        throw std::invalid_argument("Acknowledgment Type");
}

bool isHead(const cMessage *msg)
{
    return ((msg->getKind() == HEAD) || (msg->getKind() == HEADTAIL));
}

bool isTail(const cMessage *msg, bool check_for_head_tail_messages)
{
    if (check_for_head_tail_messages) {
        return ((msg->getKind() == TAIL) || (msg->getKind() == HEADTAIL));
    } else {
        return (msg->getKind() == TAIL);
    }
}

bool isLast(const cMessage *msg, bool check_for_head_tail_messages)
{
    return isTail(msg, check_for_head_tail_messages);
}

bool isLast(const int flit_number, const int num_flit_in_packet)
{
    return flit_number == num_flit_in_packet;
}

bool isLast(const int flit_number, const HeadFlit *head)
{
    return isLast(flit_number, head->getNum_flit());
}

bool isAck(const cMessage *msg) { return msg->getKind() == ACK; }

bool isNack(const cMessage *msg)
{
    return (msg->getKind() == NACK || isCNack(msg));
}

bool isCNack(const cMessage *msg) { return msg->getKind() == CNACK; }

bool hasControlInfo(const cMessage *msg)
{
    return (isHead(msg) || isAck(msg) || isNack(msg));
}

bool releasesPort(const cMessage *msg)
{
    return (isTail(msg) || (msg->getKind() == HEADTAIL) || isAck(msg) ||
            isNack(msg));
}

} /* namespace */
