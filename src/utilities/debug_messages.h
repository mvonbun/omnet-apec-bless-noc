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
// File      : debug_messages.h
//
// Created   : 20 Apr 2017 for apec_bless_noc
// Author(s) : Michael Vonbun
// E-mail(s) : michael.vonbun@tum.de
// 

#ifndef __DEBUG_MESSAGES_INTERFACE_H_
#define __DEBUG_MESSAGES_INTERFACE_H_

#include <cstdio>
#include <iostream>

#define S1(x) #x
#define S2(x) S1(x)
#define LOCATION(x) __FILE__ ":" S2(__LINE__) ": " x

/* CONTROL ADDITIONAL OUTPUT */
/* DEBUG */
//#define DEBUGENABLE 0
/* VALIDATION */
//#define VALIDATE
/* MESSAGE DESTRUCTOR */
//#define MSGDESTRUCTORPRINTF
/* MODULE INITIALIZATION */
//#define MODULEINIPRINTF
/* MODULE FINISH */
#define MODULEFINISHPRINTF

/* DEBUG */
// configure module and message to watch
#ifdef DEBUGENABLE
/* MESSAGE to observe */
#define DEBUGMSGID 19479
#define DEBUGMODULEID 6
#endif

/* DEBUG CODE OF MODULE */
/* #ifdef DEBUGENABLE
 *     if (m_id == DEBUGMODULEID) {
 *         // DO SOMETHING
 *     }
 * #endif
 */

#define DBGMSG(fmt, ...)                                                       \
    do {                                                                       \
        if (DEBUGENABLE)                                                       \
            fprintf(stderr, "%s:%d:%s(): " fmt "\n", __FILE__, __LINE__,       \
                    __func__, __VA_ARGS__);                                    \
    } while (0)

/* VERBOSE OUTPUT */
#define VERBOSITYENABLE 1
#define VERBMSG(fmt, ...)                                                      \
    do {                                                                       \
        if (VERBOSITYENABLE > 0)                                               \
            fprintf(stdout, "%s:%d:%s(): " fmt "\n", __FILE__, __LINE__,       \
                    __func__, __VA_ARGS__);                                    \
    } while (0)

/* #define VERBOSITY 1 */
/* #define cverbose(fmt, ...) \ */
/*         do { \ */
/*             if (VERBOSITY) \ */
/*             fprintf(stderr, "%s:%d:%s(): " fmt "\n", __FILE__, __LINE__,
 * __func__, __VA_ARGS__); \ */
/*         } while (0) */
/* #define verbose(str, ...) \ */
/*         do { \ */
/*             if (VERBOSITY > 0) \ */
/*                 EV << __FILE__ << ":" << __LINE__ << ":" << __func__ << "():
 * " << str << __VA_ARGS__ << "\n"; \ */
/*             if (VERBOSITY > 1) \ */
/*                 bubble(str); \ */
/*         } while (0) */

#define PACKET(HEAD)                                                           \
    "packet<" << HEAD->getPacket_id() << "[" << HEAD->getNum_flit()            \
              << "]: " << HEAD->getSource() << "->" << HEAD->getDestination()  \
              << ">"
#define FLIT(HEAD, SEQNUM)                                                     \
    "flit<" << HEAD->getPacket_id() << ":" << HEAD->getNum_flit()              \
            << "::" << SEQNUM << ">"

#define ACKFLITMES(ACKMES)                                                     \
    "ack<" << ACKMES->getAck_packet_id() << ":" ACKMES->getSource() << "->"    \
           << ACKMES->getDestination() << ">"

/* define custom output text tags */
#define TAGADD "[add] "
#define TAGBEG "[start] "
#define TAGCL "[clear] "
#define TAGDBG "[debug] "
#define TAGDEC "[dec] "
#define TAGDEL "[delete] "
#define TAGENC "[enc] "
#define TAGEND "[end] "
#define TAGESC "[cancel] "
#define TAGFAIL "[failure] "
#define TAGGEN "[gen] "
#define TAGGET "[get] "
#define TAGINI "[init] "
#define TAGMADE "[made] "
#define TAGNABLE "[unable] "
#define TAGNEW "[new] "
#define TAGNOTEX "[~exists] "
#define TAGNOTIN "[not in] "
#define TAGPOP "[pop] "
#define TAGPUSH "[push] "
#define TAGREM "[remove] "
#define TAGRX "[rx] "
#define TAGSET "[set] "
#define TAGSUCC "[success] "
#define TAGSZ "[size] "
#define TAGSZO "[sizeof] "
#define TAGTRIG "[trigger] "
#define TAGTX "[tx] "
#define TAGUSE "[use] "

/* #define ADD_INFO  EV_INFO << ADD_TXT */
/* #define BEG_INFO  EV_INFO << BEG_TXT */
/* #define CL_INFO   EV_INFO << CL_TXT */
/* #define DEC_INFO  EV_INFO << DEC_TXT */
/* #define ENC_INFO  EV_INFO << ENC_TXT */
/* #define END_INFO  EV_INFO << END_TXT */
/* #define GEN_INFO  EV_INFO << GEN_TXT */
/* #define MAKE_INFO EV_INFO << MAKE_TXT */
/* #define POP_INFO  EV_INFO << POP_TXT */
/* #define PUSH_INFO EV_INFO << PUSH_TXT */
/* #define REM_INFO  EV_INFO << REM_TXT */
/* #define RX_INFO   EV_INFO << RX_TXT */
/* #define SZO_INFO  EV_INFO << SZO_TXT */
/* #define SZ_INFO   EV_INFO << SZ_TXT */
/* #define TRIG_INFO EV_INFO << TRIG_TXT */
/* #define TX_INFO   EV_INFO << TX_TXT */
/* #define ESC_INFO  EV_INFO << ESC_TXT */
/* #define INI_INFO  EV_INFO << INI_TXT */
/* #define SUCC_INFO EV_INFO << SUCC_TXT */
/* #define FAIL_INFO EV_INFO << FAIL_TXT */
/* #define USE_INFO  EV_INFO << USE_TXT */
/* #define GET_INFO  EV_INFO << GET_TXT */
/* #define SET_INFO  EV_INFO << SET_TXT */
/* #define NOTEX_INFO EV_INFO << NOTEX_TXT */

/* #define BUBMSG(msg)  */
/* do {							\ */
/* 	if (hasGUI())						\ */
/* 	    bubble(msg);					\ */
/* } while (0) */

#endif
