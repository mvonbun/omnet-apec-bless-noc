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
// File      : enum_utilities.h
//
// Created   : 02 Mai 2017 for apec_bless_noc
// Author(s) : Michael Vonbun
// E-mail(s) : michael.vonbun@tum.de
// 

#ifndef __ENUM_UTILITIES_H_
#define __ENUM_UTILITIES_H_

// stl
#include <string>

// omnet
#include <omnetpp.h>

// project messages
#include <noc_flits.h>

using namespace omnetpp;

namespace util
{
/** String to Int Enum conversions.
 *
 */
int convertMessageType(const std::string &str);
int convertCodeType(const std::string &str);
int convertResourceReleaseType(const std::string &str);
int convertAckType(const std::string &str);

/** Int to String Enum conversions.
 *
 */
std::string convertMessageType(const int type);
std::string convertCodeType(const int type);
std::string convertResourceReleaseType(const int type);
std::string convertAckType(const int type);

/** Message type checking
 *
 */
bool isHead(const cMessage *msg);
bool isTail(const cMessage *msg, bool check_for_head_tail_messages = false);
bool isLast(const cMessage *msg, bool check_for_head_tail_messages = false);
bool isLast(const int flit_number, const int num_flit_in_packet);
bool isLast(const int flit_number, const HeadFlit *head);
bool isAck(const cMessage *msg);
bool isNack(const cMessage *msg);
bool isCNack(const cMessage *msg);

bool hasControlInfo(const cMessage *msg);
bool releasesPort(const cMessage *msg);

} /* namespace */

#endif /* __ENUM_UTILITIES_H_ */
