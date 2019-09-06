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
// File      : router.cc
// Created   : 03 May 2017 for apec_bless_noc
// Author(s) : Michael Vonbun
// E-mail(s) : michael.vonbun@tum.de
// 

// stl
#include <algorithm>
#include <cassert>    // assert
#include <cstdio>     // printf
#include <functional> // std::bind

// project messages
#include <enum_definitions_m.h>
#include <noc_flits.h>
#include <noc_messages.h>

// project utilities
#include <add_module_statistic.h>
#include <debug_messages.h>
#include <enum_utilities.h>
#include <stl_fifo.h>

// module
#include <mesh_2d_utilities.h>

// header
#include "router.h"

BaseRouter::BaseRouter()
    : m_arbitration_event{nullptr}, m_tx_event{nullptr},
      m_are_input_ports_reset()
{
}

BaseRouter::~BaseRouter()
{
    cancelAndDelete(m_arbitration_event);
    cancelAndDelete(m_tx_event);

    for (int i = 0; i < m_port_size; ++i) {
        if (m_out_port[i].flit) {
            cancelAndDelete(m_out_port[i].flit);
        }

        if (m_in_port[i].flit) {
            delete m_in_port[i].flit;
        }

        m_in_port[i].popRequest(); // pop initial IDLE request
        m_in_port[i].flit = nullptr;
        m_out_port[i].flit = nullptr;
    }
}

void BaseRouter::initialize()
{
    // get static module parameters from ned
    m_id = par("id").longValue();
    m_t_clk = par("t_clk");
    m_num_rows = par("num_rows").longValue();
    m_num_columns = par("num_columns").longValue();
    m_row = par("row").longValue();
    m_column = par("column").longValue();
    m_delay = par("delay");
    m_num_states_threshold = par("num_states_threshold");
    ++m_num_states_threshold; // step variable for valid comparison using '<'

    // get static module parameters from cSimpleModule
    m_in_port_base_id = gateBaseId("port$i");
    m_out_port_base_id = gateBaseId("port$o");
    m_port_size = gateSize("port");
    m_inport_size = m_port_size;

    // resize ports
    m_in_port.resize(m_port_size);
    m_out_port.resize(m_port_size);

    // store gates in aliases
    for (int i = 0; i < m_port_size; i++) {
        m_in_port[i].gate = gate(m_in_port_base_id + i);
        m_out_port[i].gate = gate(m_out_port_base_id + i);
    }

    // setup arbitration event
    m_arbitration_event = new cMessage;
    m_arbitration_event->setKind(ARBEVENT);
    m_arbitration_event->setSchedulingPriority(SP_ARBITRATE);

    // setup transmission event
    m_tx_event = new cMessage;
    m_tx_event->setKind(TXEVENT);
    m_tx_event->setSchedulingPriority(SP_TRANSMIT);

    // reset
    resetRouterStates();

    // register statistic signals
    // clang-format off
    m_flit_drop_signal = util::addModuleStatistic(
            this, "router-", m_id, "_flit-drop", "router_flit-drop");
    m_ack_drop_signal = util::addModuleStatistic(
            this, "router-", m_id, "_ack-drop", "router_ack-drop");
    m_packet_drop_signal = util::addModuleStatistic(
            this, "router-", m_id, "_packet-drop", "router_packet-drop");
    m_packet_drop_distance_signal = util::addModuleStatistic(
            this, "router-", m_id, "_packet-drop-distance", "router_packet-drop-distance");
    m_nack_reroute_signal = util::addModuleStatistic(
                this, "router-", m_id, "_nack-reroute", "router_nack-reroute");
    m_cnack_number_signal = util::addModuleStatistic(
                    this, "router-", m_id, "_cnack-number", "router_cnack-number");
    // clang-format on

    // debug
    EV_DEBUG << "Scheduling priorities are: " << SP_ARBITRATE << ", "
             << SP_RELEASE << ", " << SP_TRANSMIT << "\n";

    // gui watches
    //    WATCH_VECTOR();
    //    WATCH_VECTOR(m_tx_flit);

    // initialize children
    childInitialize();
    nackInitialize();
}

void BaseRouter::finish() {}

void BaseRouter::handleMessage(cMessage *msg)
{
// add validation code here
#ifdef VALIDATE
    if (!msg->isSelfMessage()) {
        std::cout << "t=" << simTime();
        printf("\t%10s%-10s\t kind=%2d\t msgid=%-8ld\t port[%d]",
               this->getFullName(), "", msg->getKind(), msg->getId(),
               msg->getArrivalGateId() - m_in_port_base_id);
        std::cout << std::endl;
    }
#endif

// add debug code here
#ifdef DEBUGENABLE
    if (msg->getId() == DEBUGMSGID) {
        std::cout << this->getFullPath() << ":" << simTime() << std::endl;
        EV_DEBUG << "!!!\n";
    }
#endif

    // events
    if (msg->isSelfMessage()) {
        if (msg->getKind() == ARBEVENT) {
            // printPortStates();
            arbitrate();
            triggerTransmission();

        } else if (msg->getKind() == RELEVENT) {
            releasePort(check_and_cast<ResourceReleaseMsg *>(msg));
            delete msg;

        } else if (msg->getKind() == TXEVENT) {
            transmit();
            resetRouterStates();
        }

    } else {
        // get input port id
        int inport_id = msg->getArrivalGateId() - m_in_port_base_id;

        // store flit in port's input register
        storeFlit(check_and_cast<RootFlit *>(msg), inport_id);

        //        m_ctrl_flit_arrived =
        //            m_ctrl_flit_arrived || m_in_port[inport_id].is_flit_ctrl;

        // print arrived flit type to debug output
        if (m_in_port[inport_id].flit) {
            EV_DEBUG << TAGRX;
            if (m_in_port[inport_id].is_flit_head) {
                EV_DEBUG << "head";
            } else if (m_in_port[inport_id].is_flit_ack) {
                EV_DEBUG << "ack";
            } else if (m_in_port[inport_id].is_flit_nack) {
                EV_DEBUG << "nack";
            } else if (m_in_port[inport_id].is_flit_tail) {
                EV_DEBUG << "tail";
            } else {
                EV_DEBUG << "body";
            }
            EV_DEBUG << "<" << msg->getId() << ">";
            EV_DEBUG << " at port[ " << inport_id << "]\n";
        }

        // set desired output ports
        // - ctrl flits may request new connection
        // - body and tail flits use their old state
        // - holes are handled in moveFlits (i.e. not moved at all)
        if (m_in_port[inport_id].is_flit_ctrl) {
            processControlInfoAtPort(inport_id);
        }

        // trigger arbitration
        triggerArbitration();
    }
}

void BaseRouter::storeFlit(RootFlit *flit, const int inport_id)
{
    m_in_port[inport_id].storeFlit(flit); // store flit
    m_are_input_ports_reset = false;      // set input port reset status
}

// TODO: make exchangeable
void BaseRouter::processControlInfoAtPort(const int inport_id)
{
    pushRequestAtPort(inport_id);

    // debug output
    if (m_in_port[inport_id].is_flit_head) {
        EV_DEBUG << TAGRX << PACKET(check_and_cast<HeadFlit *>(
                                 m_in_port[inport_id].flit));
    } else {
        if (m_in_port[inport_id].is_flit_ack) {
            EV_DEBUG << TAGRX << "ack<"
                     << check_and_cast<AckFlit *>(m_in_port[inport_id].flit)
                            ->getDestination()
                     << ">";
        } else {
            EV_DEBUG << TAGRX << "nack<"
                     << check_and_cast<NackFlit *>(m_in_port[inport_id].flit)
                            ->getDestination()
                     << ">";
        }
    }
    EV_DEBUG << " :: new connection requested ::"
             << " port[" << inport_id << "] --> port["
             << m_in_port[inport_id].getRequest() << "]\n";
}

/** Routing algorithm.
 *
 *  Ports (see 2d-mesh ned):
 *  clock-wise port enumeration
 *  ([port][direction] : [0][local] [1][north] [2][east] [3][south] [4][west])
 */
int BaseRouter::getOutPortFromCtrlFlitAtPort(const int inport_id)
{
    // cast control flits (either headers or acks)
    int dest_id;
    if (m_in_port[inport_id].is_flit_head) {
        dest_id = check_and_cast<HeadFlit *>(m_in_port[inport_id].flit)
                      ->getDestination();
    } else {
        if (m_in_port[inport_id].is_flit_ack) {
            dest_id = check_and_cast<AckFlit *>(m_in_port[inport_id].flit)
                          ->getDestination();
        } else {
            dest_id = check_and_cast<NackFlit *>(m_in_port[inport_id].flit)
                          ->getDestination();
        }
    }

    // decide output port
    if (m_id == dest_id) {
        return 0; // local
    } else {
        int dest_col = util::getColumn(dest_id, m_num_rows, m_num_columns);
        int dest_row = util::getRow(dest_id, m_num_rows, m_num_columns);

        // XY-dimension routing
        if (m_column == dest_col) {
            if (m_row < dest_row) {
                return 3; // south
            } else {
                return 1; // north
            }
        } else {
            if (m_column < dest_col) {
                return 2; // east
            } else {
                return 4; // west
            }
        }
    }
}

void BaseRouter::triggerArbitration()
{
    if (!m_arbitration_event->isScheduled()) {
        EV_DEBUG << TAGTRIG << "arbitration\n";
        scheduleAt(simTime(), m_arbitration_event);
    }
}

void BaseRouter::arbitrate()
{
    EV_DEBUG << TAGDBG << "arbitrate :: port requests\n";
    EV_DEBUG << TAGDBG << "\t- collect port requests\n";
    collectPortRequests();
    if (m_id == 1) {
        printPortStates();
        fflush(stdout);
    }
    EV_DEBUG << TAGDBG << "\t- process port requests\n";
    processPortRequests();

    EV_DEBUG << TAGDBG << "arbitrate :: trigger releases\n";
    triggerPortReleases();

    EV_DEBUG << TAGDBG << "arbitrate :: execute\n";
    executePortStates(); // move flits from in- to output port
}

void BaseRouter::collectPortRequests()
{
    // TODO: add all requests, but only unique ones (starting from the newer
    // requests)
    size_t num_requests;
    for (int i = 0; i < m_inport_size; ++i) {
        num_requests = m_in_port[i].request.size() - 1;
        if (num_requests > 0) {
            for (size_t j = 0; j < num_requests; ++j) {
                if (m_in_port[i].getRequest(j) > IDLE) {
                    if (j == 0)
                        m_out_port[m_in_port[i].getRequest(j)]
                            .addRequestingPort(i);
                    //                m_out_port[m_in_port[i].getRequest()].requested_by.push_back(i);
                } else {
                    if (m_in_port[i].getRequest(j) == DELETE) {
                        if (j == 0) {
                            m_in_port[i].grantRequest();
                            EV_DEBUG << "DELETE granted\n";
                        }
                    } else {
                        assert(false && "Invalid REQUEST.");
                    }
                }
            }
        }
    }
}

void BaseRouter::processPortRequests()
{
    EV_DEBUG << "[check] for multiple requests\n";

    // do for all ports
    for (int i = 0; i < m_port_size; ++i) {
        if (m_out_port[i].requested_by.size() > 0) {
            if (m_out_port[i].requested_by.size() < 2) {
                // only one request, no conflict
                EV_DEBUG << "   output port[" << i << "] requested by port<"
                         << m_out_port[i].requested_by.back() << "> only.\n";

                bool port_still_requested = false;
                if (m_in_port[m_out_port[i].requested_by.back()].is_flit_head) {
                    // for one flit pakets it's ok
                    if (check_and_cast<HeadFlit *>(
                            m_in_port[m_out_port[i].requested_by.back()].flit)
                            ->getNum_baseflit() > 1) {
                        // if head, check if path is already requested by other
                        // port,
                        // but currently there is a hole (e.g. Nack delete a
                        // Body on it's way)
                        size_t num_requests;
                        for (int j = 0; j < m_inport_size; j++) {
                            if (j == m_out_port[i].requested_by.back())
                                continue;
                            num_requests = m_in_port[j].request.size() - 1;
                            if (num_requests > 0) {
                                for (size_t k = 0; k < num_requests; k++) {
                                    if (m_in_port[j].getRequest(k) == i) {
                                        port_still_requested = true;
                                    }
                                }
                            }
                        }
                    }
                }
                if (port_still_requested)
                    m_in_port[m_out_port[i].requested_by.back()].rejectRequest(
                        i);
                else
                    m_in_port[m_out_port[i].requested_by.back()].grantRequest(
                        i);
            } else {
                // multiple requests on same port: resolve

                // debug output
                EV_DEBUG << "   output port[" << i << "] requested by <"
                         << m_out_port[i].requested_by.size() << "> ports: [ ";
                for (m_out_port[i].it_requested_by =
                         m_out_port[i].requested_by.begin();
                     m_out_port[i].it_requested_by !=
                     m_out_port[i].requested_by.end();
                     ++m_out_port[i].it_requested_by) {
                    EV_DEBUG << *m_out_port[i].it_requested_by << " ";
                }
                EV_DEBUG << "]\n";

                // if more heads request Port which is already occupied, but
                // currently there is a hole
                bool only_heads = true;
                for (m_out_port[i].it_requested_by =
                         m_out_port[i].requested_by.begin();
                     m_out_port[i].it_requested_by !=
                     m_out_port[i].requested_by.end();
                     ++m_out_port[i].it_requested_by) {

                    if (!m_in_port[*m_out_port[i].it_requested_by]
                             .is_flit_head) {
                        only_heads = false;
                    }
                }

                // If not all flits are heads, no new path will be occupied,
                // continue as usual
                bool port_still_requested = false;
                if (only_heads) {
                    size_t num_requests;
                    for (int j = 0; j < m_inport_size; j++) {
                        num_requests = m_in_port[j].request.size() - 1;
                        m_out_port[i].it_requested_by =
                            std::find(m_out_port[i].requested_by.begin(),
                                      m_out_port[i].requested_by.end(), j);
                        // do not consider requesting ports --> there is no
                        // hole, except one flit pakets
                        if (m_out_port[i].it_requested_by !=
                            m_out_port[i].requested_by.end()) {
                            if (check_and_cast<HeadFlit *>(
                                    m_in_port[*m_out_port[i].it_requested_by]
                                        .flit)
                                    ->getNum_baseflit() == 1) {
                                num_requests--;
                            } else
                                continue;
                        }
                        if (num_requests > 0) {
                            for (size_t k = 0; k < num_requests; k++) {
                                if (m_in_port[j].getRequest(k) == i) {
                                    port_still_requested = true;
                                }
                            }
                        }
                    }
                }

                if (!port_still_requested) {
                    // store current output port for use in sort / compare
                    // functions
                    m_current_outport_request_processing = i;

                    // sort the requests based on their priority
                    sortPortRequests(i);

                    // debug output
                    EV_DEBUG << "   output port[" << i
                             << "] request order is: [ ";
                    for (m_out_port[i].it_requested_by =
                             m_out_port[i].requested_by.begin();
                         m_out_port[i].it_requested_by !=
                         m_out_port[i].requested_by.end();
                         ++m_out_port[i].it_requested_by) {
                        EV_DEBUG << *m_out_port[i].it_requested_by << " ";
                    }
                    EV_DEBUG << "]\n";

                    // set port states based on sort result
                    setPortStates(i);
                } else {
                    EV_DEBUG << "Only heads are requesting Port which is "
                                "already occupied by hole, reject all!";
                    for (m_out_port[i].it_requested_by =
                             m_out_port[i].requested_by.begin();
                         m_out_port[i].it_requested_by !=
                         m_out_port[i].requested_by.end();
                         ++m_out_port[i].it_requested_by) {
                        m_in_port[*m_out_port[i].it_requested_by].rejectRequest(
                            i);
                    }
                }
            }
        }
    }
}

void BaseRouter::setPortStates(const int outport_id)
{
    // first element: request granted, other rejected
    for (m_out_port[outport_id].it_requested_by =
             m_out_port[outport_id].requested_by.begin();
         m_out_port[outport_id].it_requested_by !=
         m_out_port[outport_id].requested_by.end();
         ++m_out_port[outport_id].it_requested_by) {

        if (m_out_port[outport_id].it_requested_by ==
            m_out_port[outport_id].requested_by.begin()) {
            EV_DEBUG << "   winner: port["
                     << *m_out_port[outport_id].it_requested_by << "]"
                     << " access granted to port[" << outport_id << "]"
                     << "\n";
            m_in_port[*m_out_port[outport_id].it_requested_by].grantRequest(
                outport_id);
        } else {
            EV_DEBUG << "   loser: port["
                     << *m_out_port[outport_id].it_requested_by << "]"
                     << " no access granted to port[" << outport_id << "]";
            if (m_in_port[*m_out_port[outport_id].it_requested_by].flit ==
                nullptr) {
                EV_DEBUG << " ==> hit hole, no packet deletion";
            } else {
                EV_DEBUG << " ==> flit["
                         << m_in_port[*m_out_port[outport_id].it_requested_by]
                                .flit->getId()
                         << "] will be deleted";
            }
            EV_DEBUG << "\n";

            m_in_port[*m_out_port[outport_id].it_requested_by].rejectRequest(
                outport_id);
        }
    }
}

/** Core function to sort output port requests. */
bool BaseRouter::compareRequests(const int &first, const int &second)
{
    // Plain operation (smaller is better):
    // [TAIL] < [NACK] < BODY < ACK < HEAD

    // However, it should be impossible that 2 TAILs or BODYs are compared.

    // For modularity, we use multiple subroutines.
    // This toplevel function only checks if both flits are ctrl flits or not.
    //
    // | -------- + -------- + --------------- |
    // | first    | second   | action          |
    // | -------- + -------- + --------------- |
    // | ctrl     | ctrl     | compare_ctrl    |
    // | ctrl     | non-ctrl | compare_reg     |
    // | non-ctrl | ctrl     | inv compare_reg |
    // | -------- + -------- + --------------- |

    // if both flits are control flits
    //  => pass processing to control flit compare routine
    if ((m_in_port[first].isRequestCtrl(
            m_current_outport_request_processing)) &&
        (m_in_port[second].isRequestCtrl(
            m_current_outport_request_processing))) {
        return compareCtrlRequests(first, second);

    } else {
        // else, one of both should not be a control flit
        //  => assert by checking if at least one of both is a ctrl flit
        // (as two non-ctrl requests should never ever happen, e.g TAIL - TAIL
        // or BODY - BODY )
        assert(
            (m_in_port[first].is_flit_ctrl || m_in_port[second].is_flit_ctrl));

        // return m_in_port[second].is_flit_ctrl;
        // based on which flit is the ctrl flit
        //  => pass processing to subroutine implemented by the derived routers
        if (m_in_port[first].is_flit_ctrl) {
            return compareRegularRequests(first, second);

            // second is ctrl flit
            //  => change roles of first and second and negate
        } else {
            return !compareRegularRequests(second, first);
        }
    }
}

bool BaseRouter::compareCtrlRequests(const int &first, const int &second)
{
    // preliminaries by callers:
    //  first is ctrl
    //  second is ctrl

    // Base subroutine to handle ctrl-only requests.
    // | ----- + ------ + ---------- |
    // | first | second | action     |
    // | ----- + ------ + ---------- |
    // | ack   | ack    | see below  |
    // | head  | head   | see below  |
    // | ack   | head   | true       |
    // | head  | ack    | false      |
    // | ----- + ------ + ---------- |

    // control flits are either acks or heads
    // priority order is (lower is better):
    // ack < head
    // -> packet priority
    // -> older < newer                  (reduces latency)
    // -> more_attempts < fewer_attempts (reduces amount of retransmissions)
    // -> lower_tree_id < higher_tree_id (priors nodes that send infrequently)
    // -> lower_msg_id < higher_msg_id   (priors nodes that send infrequently)
    if (((m_in_port[first].is_flit_ack) && (m_in_port[second].is_flit_ack)) ||
        ((m_in_port[first].is_flit_head) && (m_in_port[second].is_flit_head))) {
        // flit types are equal (acks or heads) => compare priorities
        if (m_in_port[first].getPriority() == m_in_port[second].getPriority()) {
            // flits have equal priorities => compare creation times
            if (m_in_port[first].flit->getCreationTime() ==
                m_in_port[second].flit->getCreationTime()) {
                // creation times are equal => compare number of attempts
                // (num_attempts member differs for acks and heads)
                int first_num_attempts, second_num_attempts;
                if (m_in_port[first].is_flit_ack) {
                    first_num_attempts =
                        check_and_cast<AckFlit *>(m_in_port[first].flit)
                            ->getPacket_attempts();
                    second_num_attempts =
                        check_and_cast<AckFlit *>(m_in_port[second].flit)
                            ->getPacket_attempts();
                } else {
                    first_num_attempts =
                        check_and_cast<HeadFlit *>(m_in_port[first].flit)
                            ->getAttempts();
                    second_num_attempts =
                        check_and_cast<HeadFlit *>(m_in_port[second].flit)
                            ->getAttempts();
                }

                if (first_num_attempts == second_num_attempts) {
                    // number of attempts are equal => compare tree id
                    if (m_in_port[first].flit->getTreeId() ==
                        m_in_port[second].flit->getTreeId()) {
                        // tree ids are equal => compare ids
                        // (ids are unique, so comparison will succeed here)
                        return m_in_port[first].flit->getId() <
                               m_in_port[second].flit->getId();
                    } else {
                        // tree ids differ:
                        // if first has a smaller tree id return true
                        // else return false
                        return m_in_port[first].flit->getTreeId() <
                               m_in_port[second].flit->getTreeId();
                    }
                } else {
                    // number of attempts differ:
                    // if first has more attempts return true
                    // else return false
                    return first_num_attempts > second_num_attempts;
                }
            } else {
                // creation times differ:
                // if first is smaller (older) return true
                // else return false
                return m_in_port[first].flit->getCreationTime() <
                       m_in_port[second].flit->getCreationTime();
            }

        } else {
            // priorities differ:
            // let subroutine decide
            return compareRequestPriorities(first, second);
        }

    } else {
        // flit types differ:
        // if first is ack return true (as it is "smaller")
        // else return false (as second is ack)
        return m_in_port[first].is_flit_ack;
    }
}

bool BaseRouter::compareRequestPriorities(const int &first, const int &second)
{
    // return m_in_port[first].getPriority() < m_in_port[second].getPriority();
    return m_in_port[first].getRequestPriority(
               m_current_outport_request_processing) <
           m_in_port[second].getRequestPriority(
               m_current_outport_request_processing);
}

void BaseRouter::executePortStates()
{
    for (int i = 0; i < m_inport_size; ++i) {
        if (m_in_port[i].flit) {
            // if a new packet gets the port, increase priority a bit (decrement
            // by one);
            // this eases the comparison of later packets with equal priority.
            // Example:
            // - p1 arrives with prio=10 and gets the (idle) port
            // - p2 arrives with prio=10 some cycles later and tries to get the
            // port
            // => as we only check for inequality, the result of the comparison
            // is random
            //    (depending on which port is checked first), so it might happen
            //    that p2 gets the port
            // => either we add an additional checking that prioritizes active
            // connections
            //    or we simply increase the priority a bit
            if (m_in_port[i].is_flit_head) {
                if (m_in_port[i].state == DELETE) {
                    m_in_port[i].rejectPacket();
                } else {
                    m_in_port[i].grantPacket();
                }
            }
            executeState(i, m_in_port[i].state);
        }
    }
}

void BaseRouter::triggerTransmission()
{
    if (!m_tx_event->isScheduled()) {
        EV_DEBUG << TAGTRIG << "transmission\n";
        scheduleAt(simTime(), m_tx_event);
    }
}

void BaseRouter::transmit()
{
    for (int i = 0; i < m_port_size; ++i) {
        if (m_out_port[i].flit) {
            EV_DEBUG << TAGTX << "flits after <" << m_delay << "> cycles\n";

            sendDelayed(m_out_port[i].flit, m_delay * m_t_clk,
                        m_out_port[i].gate);
        }
    }
}

void BaseRouter::resetRouterStates()
{
    if (!m_are_input_ports_reset) {
        for (int i = 0; i < m_port_size; ++i) {
            m_out_port[i].reset();
        }
        for (int i = 0; i < m_inport_size; ++i) {
            m_in_port[i].reset();
        }

        m_ctrl_flit_arrived = false;
        m_are_input_ports_reset = true;
    }
}

// TODO
void BaseRouter::printPortStates()
{
    for (int i = 0; i < m_inport_size; ++i) {
        printPortState(i);
        //        EV_DEBUG << "input port[" << i << "]\n";
        //
        //        EV_DEBUG << "   has <" << m_in_port[i].state.size() << ">
        //        states:\n";
        //        for (size_t j = 0; j < m_in_port[i].state.size(); ++j) {
        //            EV_DEBUG << "      <" << j << ">::";
        //            EV_DEBUG << " state<" << m_in_port[i].state[j] << ">";
        //            EV_DEBUG << " time<" << m_in_port[i].timestamp[j] <<
        //            ">\n";
        //        }
        //
        //        if (m_in_port[i].flit) {
        //            EV_DEBUG << "   has a "
        //                     <<
        //                     util::convertMessageType(m_in_port[i].flit->getKind())
        //                     << " flit\n";
        //            EV_DEBUG << "   that requested[" <<
        //            m_in_port[i].requested_port
        //                     << "]\n";
        //        } else {
        //            EV_DEBUG << "   is empty\n";
        //        }
        //        EV_DEBUG << "   with current state <" <<
        //        m_in_port[i].top_state()
        //                 << ">";
        //        EV_DEBUG << " pushed at the stack at t="
        //                 << m_in_port[i].timestamp.back() << "\n";
        //    }
        //
        //    for (int i = 0; i < m_port_size; ++i) {
        //        EV_DEBUG << "output port[" << i << "]\n";
        //        EV_DEBUG << "   has <" << m_out_port[i].state.size() << ">
        //        states:\n";
        //        if (m_out_port[i].state.size() > 0) {
        //            EV_DEBUG << "   with current state <" <<
        //            m_out_port[i].top_state()
        //                     << ">";
        //            EV_DEBUG << " pushed at the stack at t="
        //                     << m_out_port[i].timestamp.back() << "\n";
        //        }
        //
        //        for (size_t j = 0; j < m_out_port[i].state.size(); ++j) {
        //            EV_DEBUG << "      <" << j << ">::";
        //            EV_DEBUG << " state<" << m_out_port[i].state[j] << ">";
        //            EV_DEBUG << " time<" << m_out_port[i].timestamp[j] <<
        //            ">\n";
        //        }
    }
}

void BaseRouter::printPortState(const int port_id,
                                const bool port_type_is_input)
{
    EV_DEBUG << "input port[" << port_id << "] state\n";
    EV_DEBUG << "  current state: " << m_in_port[port_id].state << "\n";
    EV_DEBUG << "  requests @ t\n";
    for (size_t i = 0; i < m_in_port[port_id].request.size(); ++i) {
        EV_DEBUG << "  " << m_in_port[port_id].request[i] << " @ "
                 << m_in_port[port_id].timestamp[i] << "\n";
    }
}

void BaseRouter::recordPacketDrop(RootFlit *flit)
{
    recordPacketDrop(check_and_cast<HeadFlit *>(flit));
}

void BaseRouter::recordPacketDrop(HeadFlit *head)
{
    int distance_traveled = abs(head->getSource() / m_num_columns - m_row) +
                            abs(head->getSource() % m_num_columns - m_column);
    EV_DEBUG << TAGDEL << "packet header of " << PACKET(head) << " after <"
             << distance_traveled << "> hops\n";
    emit(m_packet_drop_signal, head->getNum_flit());
    emit(m_packet_drop_distance_signal, distance_traveled);
}

/** Plain router implementation. */
void PlainRouter::childInitialize() {}

void PlainRouter::pushRequestAtPort(const int inport_id)
{
    m_in_port[inport_id].pushRequest(getOutPortFromCtrlFlitAtPort(inport_id));
}

void PlainRouter::sortPortRequests(const int outport_id)
{
    using namespace std::placeholders;
    m_out_port[outport_id].requested_by.sort(
        std::bind(&PlainRouter::compareRequests, this, _1, _2));
}

void PlainRouter::executeState(const int port, const int state)
{
    // if in_port state is discard, delete the flits
    // Also delete if state = NOREQUEST, to avoid crash if release triggered to
    // early
    if (state == DELETE || state == NOREQUEST) {
        EV_DEBUG << TAGDEL << "flit<"
                 << m_in_port[port].flit->getSequence_number() << "> at port["
                 << port << "]\n";

        // record dropped flits
        emit(m_flit_drop_signal, true);

        // clang-format off
        // record dropped packets
        if (m_in_port[port].is_flit_head) {
            recordPacketDrop(m_in_port[port].flit);
        }

        // record dropped acks
        if (m_in_port[port].is_flit_ack) {
            emit(m_ack_drop_signal,
                check_and_cast<AckFlit *>(m_in_port[port].flit)->getPacket_num_flit());
        }
        // clang-format on

        delete m_in_port[port].flit;
        m_in_port[port].flit = nullptr;

        // otherwise forward the flits to the output registers
    } else {
        // TODO: it is here where the problem occurs
        assert(state > -1);
        EV_DEBUG << TAGADD << "flit<"
                 << m_in_port[port].flit->getSequence_number() << ">";
        EV_DEBUG << " from in-port[" << port << "]";
        EV_DEBUG << " to out-port[" << state << "]\n";

        m_out_port[state].flit = m_in_port[port].flit;
        m_in_port[port].flit = nullptr;
    }
}

/** Plain router using tail-flag to release resources. */
void FlagRouter::triggerPortReleases()
{
    // for all tail-like received flits (that can release ports)
    for (int i = 0; i < m_inport_size; ++i) {
        if (m_in_port[i].is_flit_release && !m_in_port[i].release_triggered) {
            // release port based on the own state
            ResourceReleaseMsg *msg = new ResourceReleaseMsg(
                i, SP_RELEASE, m_in_port[i].flit->getId());
            scheduleAt(simTime(), msg);
            m_in_port[i].release_triggered = true;
        }
    }
}

void PlainRouter::releasePort(ResourceReleaseMsg *msg)
{
    int port_id = msg->getPort_id();

    // remove own input port request
    EV_DEBUG << TAGPOP << "request[" << m_in_port[port_id].getRequest() << "]"
             << " from input port[" << port_id << "]\n";
    m_in_port[port_id].popRequest();
}

bool PlainRouter::compareRegularRequests(const int &first, const int &second)
{
    // preliminaries by callers:
    //  first is ctrl
    //  second is non-ctrl

    // ctrl flits always loose:
    // | -------- + -------- + ---------- |
    // | first    | second   | action     |
    // | -------- + -------- + ---------- |
    // | ctrl     | non-ctrl | false      |
    // | non-ctrl | ctrl     | true       |
    // | -------- + -------- + ---------- |
    // return m_in_port[second].is_flit_ctrl;
    return m_in_port[second].isRequestCtrl(
        m_current_outport_request_processing);
}

void FlagRouter::validatePortStates()
{
    // Input ports have at most 1 active connection
    // (as we push the idle state at initialization,
    // this leaves us with 2 states at most).
    // Output ports have at most 1 active connection as well.
    for (int i = 0; i < m_inport_size; ++i) {
        assert(m_in_port[i].request.size() < (m_num_states_threshold + 1));
    }
    for (int i = 0; i < m_port_size; ++i) {
        assert(m_out_port[i].requested_by.size() < m_num_states_threshold);
    }
}

/** Plain router using packet length to release resources. */
void LengthRouter::triggerPortReleases()
{
    // for all received control flits (that can set port states)
    for (int i = 0; i < m_inport_size; ++i) {
        if (m_in_port[i].is_flit_ctrl && !m_in_port[i].release_triggered) {
            ResourceReleaseMsg *msg = new ResourceReleaseMsg(
                i, SP_RELEASE, m_in_port[i].flit->getId());

            m_in_port[i].release_triggered = true;

            if (m_in_port[i].is_flit_head) {
                // One-flit packets release the port immediately,
                // regular packets use the packet length.
                // If one-flit packets are coded, and therefore have a
                // packet length > 1, the headers are treated separately as
                // individual packets. Therefore, irrespective of the packet
                // length, the delay is 0 in this case.
                int cycles_until_release =
                    (m_in_port[i].is_flit_tail)
                        ? (0)
                        : ((check_and_cast<HeadFlit *>(m_in_port[i].flit)
                                ->getNum_flit() -
                            1));
                scheduleAt(simTime() + m_t_clk * cycles_until_release, msg);

                EV_DEBUG << TAGTRIG << "release port[" << i << "]"
                         << " claimed by head<" << m_in_port[i].flit->getId()
                         << ">"
                         << " after <" << cycles_until_release << ">cycles\n";

            } else {
                if (m_in_port[i].is_flit_ack) {
                    EV_DEBUG << TAGTRIG << "release port[" << i << "]"
                             << " claimed by ack<" << m_in_port[i].flit->getId()
                             << ">"
                             << " immediately\n";
                    scheduleAt(simTime(), msg);
                } else {
                    assert(m_in_port[i].is_flit_nack);
                    EV_DEBUG << TAGTRIG << "release port[" << i << "]"
                             << " claimed by nack<"
                             << m_in_port[i].flit->getId() << ">"
                             << " immediately\n";
                    scheduleAt(simTime(), msg);
                }
            }
        }
    }
}

/** Coded router implementation. */
void CodedRouter::childInitialize()
{
    // module parameter
    m_supplant_acks_only = par("supplant_acks_only");
    m_supplant_packet_length_threshold = par("supplant_threshold");

    // resize members
    //    m_in_port_supplant.resize(m_port_size);

    // register statistic signals
    // clang-format off
    m_supplant_signal = util::addModuleStatistic(
            this, "router-", m_id, "_supplant", "router_supplant");
    // clang-format on
}

bool CodedRouter::compareRegularRequests(const int &first, const int &second)
{
    // preliminaries by callers:
    //  first is ctrl
    //  second is non-ctrl
    if (m_in_port[second].is_flit_tail && isFlagRouter()) {
        return false;
    } else {
        if (m_supplant_packet_length_threshold > 0) {
            if (m_supplant_acks_only) {
                return m_in_port[first].is_flit_ack;

            } else { // supplant also real packets
                if (m_in_port[first].is_flit_ack) {
                    return true;
                }
                HeadFlit *head =
                    check_and_cast<HeadFlit *>(m_in_port[first].flit);
                // acks and one-flit packets can supplant
                if (head->getNum_baseflit() == 1) {
                    // reduze priority so that other heads can not win
                    size_t sz = m_in_port[first].priority.size();
                    m_in_port[first].priority[sz - 1] =
                        m_in_port[first].priority[sz - 1] - 10;
                    return true;

                } else { // if length does not exceed threshold, packet can
                         // supplant
                    if (head->getNum_flit() <=
                        m_supplant_packet_length_threshold) {
                        size_t sz = m_in_port[first].priority.size();
                        m_in_port[first].priority[sz - 1] =
                            m_in_port[first].priority[sz - 1] - 10;
                        return true;
                    } else
                        return false;
                }
            }
        } else { // supplanting is disabled
            return false;
        }
    }
}

/** Nack Router implementation */
NackRouter::~NackRouter()
{
    for (int i = m_port_size; i < m_inport_size; ++i) {
        if (m_in_port[i].flit) {
            delete m_in_port[i].flit;
        }

        m_in_port[i].popRequest(); // pop initial IDLE request
        m_in_port[i].flit = nullptr;
    }
}

void NackRouter::nackInitialize()
{
    m_inport_size = 2 * m_port_size; // add virtual inports

    m_in_port.resize(m_inport_size);

    // store gates in aliases
    for (int i = m_port_size; i < m_inport_size; i++) {
        m_in_port[i].gate = nullptr; // virtual ports, no real gates
        m_in_port[i].reset();
    }
}

void NackRouter::storeFlit(RootFlit *flit, const int inport_id)
{
    if (util::isCNack(flit)) {
        // print arrived flit type to debug output
        EV_DEBUG << TAGRX << "cNack at port[" << inport_id << "]\n";
        // Flit is cNack --> split it to virtual inports

        // first extract nacks from cnack
        CNackFlit *cnack = check_and_cast<CNackFlit *>(flit);
        int numberOfNacks = cnack->getNum_nacks_combined();
        emit(m_cnack_number_signal, numberOfNacks);
        std::vector<NackFlit *> nack;
        nack.resize(numberOfNacks);
        for (int i = 0; i < numberOfNacks; i++) {
            nack[i] = cnack->popNack();
            // change ownership
            take(nack[i]);
        }
        assert(cnack->getNum_nacks_combined() ==
               0); // all nacks should be stored in nack[]

        // cnack is not needed anymore
        delete cnack;
        cnack = nullptr;

        // separate output ports
        std::list<NackFlit *> dest_port[m_port_size];
        for (int i = 0; i < numberOfNacks; i++) {
            int dest = getOutPortForNack(nack[i]);
            dest_port[dest].push_back(nack[i]);
        }

        // store nacks in virtual inports
        for (int i = 0; i < m_port_size; i++) {
            if (dest_port[i].size() > 0) {
                if (dest_port[i].size() < 2) {
                    // only one nack
                    if (m_in_port[m_port_size + i].flit) {
                        // nack already in inport --> cNack
                        CNackFlit *tmpCNack = genCNack(
                            m_in_port[m_port_size + i].flit,
                            check_and_cast<RootFlit *>(dest_port[i].back()));
                        m_in_port[m_port_size + i].storeFlit(
                            check_and_cast<RootFlit *>(tmpCNack));
                    } else {
                        // no flit in virtual inport, just store it
                        m_in_port[m_port_size + i].storeFlit(
                            check_and_cast<RootFlit *>(dest_port[i].back()));
                    }
                    dest_port[i].pop_back();
                } else {
                    // two or more --> again cNack
                    NackFlit *tmpNack = dest_port[i].back();
                    dest_port[i].pop_back();
                    NackFlit *tmpNack2 = dest_port[i].back();
                    dest_port[i].pop_back();
                    CNackFlit *tmpCNack = genCNack(tmpNack, tmpNack2);
                    size_t prev_size = dest_port[i].size();
                    for (size_t j = 0; j < prev_size; j++) {
                        tmpNack = dest_port[i].back();
                        dest_port[i].pop_back();
                        tmpCNack = genCNack(tmpCNack, tmpNack);
                    }

                    if (m_in_port[m_port_size + i].flit) {
                        // nack already in inport --> cNack
                        tmpCNack =
                            genCNack(m_in_port[m_port_size + i].flit,
                                     check_and_cast<RootFlit *>(tmpCNack));
                        m_in_port[m_port_size + i].storeFlit(
                            check_and_cast<RootFlit *>(tmpCNack));
                    } else {
                        // no flit in virtual inport, just store generated cNack
                        m_in_port[m_port_size + i].storeFlit(
                            check_and_cast<RootFlit *>(tmpCNack));
                    }
                }
            }
            assert(dest_port[i].size() == 0); // all nacks handled?
            // processCtrlInfor for virtual ports
            if (m_in_port[m_port_size + i].flit &&
                m_in_port[m_port_size + i].request.size() < 2) {
                m_in_port[m_port_size + i].pushRequest(i);

                // debug output
                if (m_in_port[m_port_size + i].is_flit_cnack) {
                    EV_DEBUG << TAGRX << "cnack";
                } else {
                    EV_DEBUG << TAGRX << "nack<"
                             << check_and_cast<NackFlit *>(
                                    m_in_port[m_port_size + i].flit)
                                    ->getDestination()
                             << ">";
                }
                EV_DEBUG << " :: new connection requested ::"
                         << " port[" << inport_id << "] --> port["
                         << m_in_port[m_port_size + i].getRequest() << "]\n";
            }
        }
    } else {
        // normal flit, just store it
        m_in_port[inport_id].storeFlit(flit); // store flit
    }
    m_are_input_ports_reset = false; // set input port reset status
}

CNackFlit *NackRouter::genCNack(RootFlit *first, RootFlit *second)
{
    CNackFlit *cNack = new CNackFlit();

    if (util::isCNack(first)) {
        cNack->pushNack(check_and_cast<CNackFlit *>(first));
        // old cNack isn't needed anymore
        delete first;
        first = nullptr;
    } else {
        cNack->pushNack(check_and_cast<NackFlit *>(first));
    }

    if (util::isCNack(second)) {
        cNack->pushNack(check_and_cast<CNackFlit *>(second));
        // old cNack isn't needed anymore
        delete second;
        second = nullptr;
    } else {
        cNack->pushNack(check_and_cast<NackFlit *>(second));
    }

    return cNack;
}

int NackRouter::getOutPortForNack(NackFlit *nack)
{
    // get dest_id
    int dest_id = nack->getDestination();

    // decide output port
    if (m_id == dest_id) {
        return 0; // local
    } else {
        int dest_col = util::getColumn(dest_id, m_num_rows, m_num_columns);
        int dest_row = util::getRow(dest_id, m_num_rows, m_num_columns);

        // XY-dimension routing
        if (m_column == dest_col) {
            if (m_row < dest_row) {
                return 3; // south
            } else {
                return 1; // north
            }
        } else {
            if (m_column < dest_col) {
                return 2; // east
            } else {
                return 4; // west
            }
        }
    }
}

void NackRouter::genNack(HeadFlit *head, int head_inport_id)
{
    EV_DEBUG << TAGGEN << " nack for " << PACKET(head) << "\n";
    // HeadFlit *port_head =
    //    check_and_cast<HeadFlit *>(m_in_port[head_inport_id].flit);
    // assert(head == port_head && "heads differ");

    NackFlit *nack = new NackFlit(head, NACK);

    storeNackFlit(nack, head_inport_id);
    processControlInfoAtPort(head_inport_id);

    // record packet drop
    recordPacketDrop(head);

    // finally delete head
    delete head;
}

void NackRouter::storeNackFlit(NackFlit *nack, const int inport_id)
{
    // free input port of head flit (is used for the nack instead)
    // delete m_in_port[inport_id].flit; // deperecated, performed by genNack
    m_in_port[inport_id].flit = nullptr;
    m_in_port[inport_id].reset();

    // set nack generation (== injection) time
    nack->setInjection_time(simTime());

    // store new nack in input port of deleted header
    m_in_port[inport_id].storeFlit(check_and_cast<RootFlit *>(nack));
}

void NackRouter::executePortStates()
{
    for (int i = 0; i < m_port_size; ++i) {
        if (m_in_port[i].flit) {
            // if a new packet gets the port, increase priority a bit (decrement
            // by one);
            // this eases the comparison of later packets with equal priority.
            // Example:
            // - p1 arrives with prio=10 and gets the (idle) port
            // - p2 arrives with prio=10 some cycles later and tries to get the
            // port
            // => as we only check for inequality, the result of the comparison
            // is random
            //    (depending on which port is checked first), so it might happen
            //    that p2 gets the port
            // => either we add an additional checking that prioritizes active
            // connections
            //    or we simply increase the priority a bit
            if (m_in_port[i].is_flit_head) {
                if (m_in_port[i].state == DELETE) {
                    m_in_port[i].rejectPacket();

                    genNack(check_and_cast<HeadFlit *>(m_in_port[i].flit), i);

                    for (int j = 0; j < m_port_size; j++)
                        m_out_port[j].reset();

                    arbitrate();
                    return;
                } else {
                    m_in_port[i].grantPacket();
                }
            }
        }
    }
    for (int i = 0; i < m_inport_size; i++) {
        if (m_in_port[i].flit) {
            executeState(i, m_in_port[i].state);
        }
    }
}

void NackRouter::setPortStates(const int outport_id)
{
    // first element: request granted, other rejected except it's a nack
    for (m_out_port[outport_id].it_requested_by =
             m_out_port[outport_id].requested_by.begin();
         m_out_port[outport_id].it_requested_by !=
         m_out_port[outport_id].requested_by.end();
         ++m_out_port[outport_id].it_requested_by) {

        if (m_out_port[outport_id].it_requested_by ==
            m_out_port[outport_id].requested_by.begin()) {
            EV_DEBUG << "   winner: port["
                     << *m_out_port[outport_id].it_requested_by << "]"
                     << " access granted to port[" << outport_id << "]"
                     << "\n";
            m_in_port[*m_out_port[outport_id].it_requested_by].grantRequest(
                outport_id);
        } else {
            if (!m_in_port[*m_out_port[outport_id].it_requested_by]
                     .is_flit_nack &&
                m_in_port[*m_out_port[outport_id].it_requested_by]
                    .is_flit_cnack) {
                EV_DEBUG << "port[" << *m_out_port[outport_id].it_requested_by
                         << "]"
                         << "combined to cNack!";
                m_in_port[*m_out_port[outport_id].it_requested_by]
                    .rejectRequest(outport_id);
            } else {
                EV_DEBUG << "   loser: port["
                         << *m_out_port[outport_id].it_requested_by << "]"
                         << " no access granted to port[" << outport_id << "]";
                if (m_in_port[*m_out_port[outport_id].it_requested_by].flit ==
                    nullptr) {
                    EV_DEBUG << " ==> hit hole, no packet deletion";
                } else {
                    if (m_in_port[*m_out_port[outport_id].it_requested_by]
                            .is_flit_nack) {
                        EV_DEBUG << " ==> nack-flit["
                                 << m_in_port[*m_out_port[outport_id]
                                                   .it_requested_by]
                                        .flit->getId()
                                 << "] will be rerouted!";
                    } else {
                        EV_DEBUG << " ==> flit["
                                 << m_in_port[*m_out_port[outport_id]
                                                   .it_requested_by]
                                        .flit->getId()
                                 << "] will be deleted";
                    }
                }
                EV_DEBUG << "\n";

                if (m_in_port[*m_out_port[outport_id].it_requested_by]
                        .is_flit_nack) {
                    // this case should only happen, if first in outport is tail
                    // flit in flag mode
                    assert(
                        m_in_port[*m_out_port[outport_id].requested_by.begin()]
                            .is_flit_tail &&
                        isFlagRouter());
                    reroute(*m_out_port[outport_id].it_requested_by);
                } else {
                    m_in_port[*m_out_port[outport_id].it_requested_by]
                        .rejectRequest(outport_id);
                }
            }
        }
    }
}

// 0 local; 1 north; 2 east; 3 south; 4 west
void NackRouter::reroute(const int inport_id)
{
    // method should only be called by nack-flits
    assert(m_in_port[inport_id].is_flit_nack);

    // (1) should never happen, that all outports are occupied if there is a
    // collision,
    // as there are max 5 incoming messages and 5 outgoing ports.
    // or it's possible to resolve by generating combined Nack.

    int dest_id;
    int new_out_port = IDLE;

    if (m_in_port[inport_id].is_flit_cnack) {
        dest_id = check_and_cast<CNackFlit *>(m_in_port[inport_id].flit)
                      ->getNackVector()[0]
                      ->getDestination();
    } else
        dest_id = check_and_cast<NackFlit *>(m_in_port[inport_id].flit)
                      ->getDestination();

    int dest_row = util::getRow(dest_id, m_num_rows, m_num_columns);

    // first try reroute by YX-routing instead of XY-routing --> would still be
    // a optimal route
    if (m_row < dest_row) {
        // try south
        if (m_out_port[3].requested_by.size() == 0 ||
            !m_in_port[*m_out_port[3].requested_by.begin()].flit) {
            new_out_port = 3;
        } else {
            // south is still possible if outport is occupied by nack --> cNack
            if (m_in_port[*m_out_port[3].requested_by.begin()].is_flit_nack) {
                genCNackCollided(*m_out_port[3].requested_by.begin(),
                                 inport_id);
                new_out_port = DELETE;
            }
        }
    } else {
        if (m_row > dest_row) {
            // try north
            if (m_out_port[1].requested_by.size() == 0 ||
                !m_in_port[*m_out_port[1].requested_by.begin()].flit) {
                new_out_port = 1;
            } else {
                // south is still possible if outport is occupied by nack -->
                // cNack
                if (m_in_port[*m_out_port[1].requested_by.begin()]
                        .is_flit_nack) {
                    genCNackCollided(*m_out_port[1].requested_by.begin(),
                                     inport_id);
                    new_out_port = DELETE;
                }
            }
        }
    }

    if (new_out_port == IDLE) {
        // optimal routing is not possible, as it's already in dest_row or
        // Y-direction is already occupied
        // just try all ports
        for (int i = m_port_size - 1; i >= 0; --i) {
            if (m_out_port[i].requested_by.size() == 0 ||
                !m_in_port[*m_out_port[i].requested_by.begin()].flit) {
                // don't use port at network border (no need to check local
                // port)
                if (i == 1 && m_row == 0)
                    continue;
                if (i == 2 && m_column == m_num_columns - 1)
                    continue;
                if (i == 3 && m_row == m_num_rows - 1)
                    continue;
                if (i == 4 && m_column == 0) {
                    continue;
                }
                new_out_port = i;
                break;
            } else {
                if (m_in_port[*m_out_port[i].requested_by.begin()]
                        .is_flit_nack) {
                    // can't be port at network border, because it's already
                    // occupied by nack
                    genCNackCollided(*m_out_port[i].requested_by.begin(),
                                     inport_id);
                    new_out_port = DELETE;
                    break;
                }
            }
        }
    }

    assert(new_out_port != IDLE); // if this happen --> no outport found -->
                                  // should never happen, see (1)

    if (new_out_port > IDLE)
        m_out_port[new_out_port].addRequestingPort(inport_id);

    m_in_port[inport_id].state = new_out_port;
    m_in_port[inport_id].request.back() = new_out_port;

    // Statistic
    emit(m_nack_reroute_signal, true);
}

bool NackRouter::compareCtrlRequests(const int &first, const int &second)
{
    // preliminaries by callers:
    //  first is ctrl
    //  second is ctrl

    // Both are nacks --> generate cNack
    if ((m_in_port[first].is_flit_nack) && (m_in_port[second].is_flit_nack)) {
        genCNackCollided(first, second);
        return true;
    }

    // Base subroutine to handle ctrl-only requests.
    // | ----- + ------ + ---------- |
    // | first | second | action     |
    // | ----- + ------ + ---------- |
    // | head  | head   | see below  |
    // | ----- + ------ + ---------- |
    // control flits are either nacks or heads
    // priority order is (lower is better):
    // nack < head
    // -> packet priority
    // -> older < newer                  (reduces latency)
    // -> more_attempts < fewer_attempts (reduces amount of retransmissions)
    // -> lower_tree_id < higher_tree_id (priors nodes that send infrequently)
    // -> lower_msg_id < higher_msg_id   (priors nodes that send infrequently)
    if ((m_in_port[first].is_flit_head) && (m_in_port[second].is_flit_head)) {
        // flit types are equal (acks or heads) => compare priorities
        if (m_in_port[first].getPriority() == m_in_port[second].getPriority()) {
            // flits have equal priorities => compare creation times
            if (m_in_port[first].flit->getCreationTime() ==
                m_in_port[second].flit->getCreationTime()) {
                // creation times are equal => compare number of attempts
                // (num_attempts member differs for acks and heads)
                int first_num_attempts, second_num_attempts;
                first_num_attempts =
                    check_and_cast<HeadFlit *>(m_in_port[first].flit)
                        ->getAttempts();
                second_num_attempts =
                    check_and_cast<HeadFlit *>(m_in_port[second].flit)
                        ->getAttempts();

                if (first_num_attempts == second_num_attempts) {
                    // number of attempts are equal => compare tree id
                    if (m_in_port[first].flit->getTreeId() ==
                        m_in_port[second].flit->getTreeId()) {
                        // tree ids are equal => compare ids
                        // (ids are unique, so comparison will succeed here)
                        return m_in_port[first].flit->getId() <
                               m_in_port[second].flit->getId();
                    } else {
                        // tree ids differ:
                        // if first has a smaller tree id return true
                        // else return false
                        return m_in_port[first].flit->getTreeId() <
                               m_in_port[second].flit->getTreeId();
                    }
                } else {
                    // number of attempts differ:
                    // if first has more attempts return true
                    // else return false
                    return first_num_attempts > second_num_attempts;
                }
            } else {
                // creation times differ:
                // if first is smaller (older) return true
                // else return false
                return m_in_port[first].flit->getCreationTime() <
                       m_in_port[second].flit->getCreationTime();
            }
        } else {
            // priorities differ:
            // let subroutine decide
            return compareRequestPriorities(first, second);
        }

    } else {
        // flit types differ:
        // | -------------- + --------------- + ------ |
        // |     first      |     second      | action |
        // | -------------- + --------------- + ------ |
        // | cnack && !nack |        *        | false  |
        // |        *       | cnack && !nack  | true   |
        // |      head      |      nack       | false  |
        // |      nack      |      head       | true   |
        // | -------------- + --------------- + ------ |
        if ((m_in_port[first].is_flit_cnack &&
             !m_in_port[first].is_flit_nack)) {
            // first is cnack but no nack --> was previously a nack which is now
            // a combined nack, always loses
            return false;
        } else {
            if (m_in_port[second].is_flit_cnack &&
                !m_in_port[second].is_flit_nack) {
                return true;
            } else {
                // if first is nack return true (as it is "smaller")
                // else return false (as second is nack)
                return m_in_port[first].is_flit_nack;
            }
        }
    }
}

void NackRouter::genCNackCollided(const int first, const int second)
{
    EV_DEBUG << "Generate combined Nack\n";

    CNackFlit *cNack = new CNackFlit();

    // Release Port before combining
    if (!m_in_port[first].release_triggered) {
        // release port based on the own state
        ResourceReleaseMsg *msg = new ResourceReleaseMsg(
            first, SP_RELEASE, m_in_port[first].flit->getId());
        scheduleAt(simTime(), msg);
        m_in_port[first].release_triggered = true;
    }
    if (!m_in_port[second].release_triggered) {
        // release port based on the own state
        ResourceReleaseMsg *msg = new ResourceReleaseMsg(
            second, SP_RELEASE, m_in_port[second].flit->getId());
        scheduleAt(simTime(), msg);
        m_in_port[second].release_triggered = true;
    }

    if (m_in_port[first].is_flit_cnack) {
        cNack->pushNack(check_and_cast<CNackFlit *>(m_in_port[first].flit));
        delete m_in_port[first].flit;
    } else
        cNack->pushNack(check_and_cast<NackFlit *>(m_in_port[first].flit));

    if (m_in_port[second].is_flit_cnack) {
        cNack->pushNack(check_and_cast<CNackFlit *>(m_in_port[second].flit));
        delete m_in_port[second].flit;
    } else
        cNack->pushNack(check_and_cast<NackFlit *>(m_in_port[second].flit));

    m_in_port[first].flit = check_and_cast<RootFlit *>(cNack);
    m_in_port[first].is_flit_cnack = true;
    m_in_port[second].is_flit_nack = false;
    m_in_port[second].is_flit_cnack = true;
    m_in_port[second].flit = nullptr;
}

// compareRegularRequests implementation for different routers
bool PlainFlagNackRouter::compareRegularRequests(const int &first,
                                                 const int &second)
{
    // preliminaries by callers:
    // first is ctrl and second is non-ctrl

    // tail (non-ctrl) < NACK (ctrl) < non-ctrl < ctrl
    if (!m_in_port[second].is_flit_tail)
        return m_in_port[first].is_flit_nack;
    else {
        // if first is nack and second is tail, both must not get lost!
        // tail wins --> nack alternative route
        return false;
    }
}

bool PlainLengthNackRouter::compareRegularRequests(const int &first,
                                                   const int &second)
{
    // preliminaries by callers:
    // first is ctrl and second is non-ctrl

    // NACK (ctrl) < non-ctrl < ctrl
    return m_in_port[first].is_flit_nack;
}

bool CodedFlagNackRouter::compareRegularRequests(const int &first,
                                                 const int &second)
{
    // preliminaries by callers:
    // first is ctrl and second is non-ctrl

    // tail (non-ctrl) < NACK (ctrl) < non-ctrl < ctrl
    // if flit ist nullptr --> flit is now a cNack
    if (m_in_port[second].is_flit_tail || m_in_port[first].flit == nullptr) {
        return false;
    } else {
        if (m_in_port[first].is_flit_nack)
            return true;
        else {
            if (m_supplant_packet_length_threshold > 0) {
                if (m_supplant_acks_only) {
                    return m_in_port[first].is_flit_ack;

                } else { // supplant also real packets
                    if (m_in_port[first].is_flit_ack) {
                        return true;
                    }
                    HeadFlit *head =
                        check_and_cast<HeadFlit *>(m_in_port[first].flit);
                    // acks and one-flit packets can supplant
                    if (head->getNum_baseflit() == 1) {
                        // reduze priority so that other heads can not win
                        size_t sz = m_in_port[first].priority.size();
                        m_in_port[first].priority[sz - 1] =
                            m_in_port[first].priority[sz - 1] - 10;
                        return true;

                    } else { // if length does not exceed threshold, packet can
                        // supplant
                        if (head->getNum_flit() <=
                            m_supplant_packet_length_threshold) {
                            size_t sz = m_in_port[first].priority.size();
                            m_in_port[first].priority[sz - 1] =
                                m_in_port[first].priority[sz - 1] - 10;
                            return true;
                        } else
                            return false;
                    }
                }
            } else { // supplanting is disabled
                return false;
            }
        }
    }
}

bool CodedLengthNackRouter::compareRegularRequests(const int &first,
                                                   const int &second)
{
    // preliminaries by callers:
    // first is ctrl and second is non-ctrl

    // if flit ist nullptr --> flit is now a cNack
    if (m_in_port[first].flit == nullptr) {
        return false;
    }
    // NACK (ctrl) < non-ctrl < ctrl
    if (m_in_port[first].is_flit_nack)
        return true;
    else {
        if (m_supplant_packet_length_threshold > 0) {
            if (m_supplant_acks_only) {
                return m_in_port[first].is_flit_ack;

            } else { // supplant also real packets
                if (m_in_port[first].is_flit_ack) {
                    return true;
                }
                HeadFlit *head =
                    check_and_cast<HeadFlit *>(m_in_port[first].flit);
                // acks and one-flit packets can supplant
                if (head->getNum_baseflit() == 1) {
                    // reduze priority so that other heads can not win
                    size_t sz = m_in_port[first].priority.size();
                    m_in_port[first].priority[sz - 1] =
                        m_in_port[first].priority[sz - 1] - 10;
                    return true;

                } else { // if length does not exceed threshold, packet can
                         // supplant
                    if (head->getNum_flit() <=
                        m_supplant_packet_length_threshold) {
                        size_t sz = m_in_port[first].priority.size();
                        m_in_port[first].priority[sz - 1] =
                            m_in_port[first].priority[sz - 1] - 10;

                        return true;
                    } else
                        return false;
                }
            }
        } else { // supplanting is disabled
            return false;
        }
    }
}

Define_Module(PlainFlagAckRouter);
Define_Module(PlainFlagNackRouter);
Define_Module(PlainLengthAckRouter);
Define_Module(PlainLengthNackRouter);
Define_Module(CodedFlagAckRouter);
Define_Module(CodedFlagNackRouter);
Define_Module(CodedLengthAckRouter);
Define_Module(CodedLengthNackRouter);
//
// void CodedRouter::validatePortStates()
//{
//    // Input ports have at most
//    //  (num_flit_max + num_code_flit)
//    // active connections.
//    // This threshold is stored via the module interface.
//    // The number of states for input ports is one more,
//    // as we push the idle state upon initialization.
//    // For output ports, we use the same threshold.
//    for (int i = 0; i < m_port_size; ++i) {
//        assert(m_in_port[i].state.size() < (m_num_states_threshold + 1));
//        assert(m_out_port[i].state.size() < m_num_states_threshold);
//    }
//}
//
// void CodedRouter::setPortStates()
//{
//    // for all received flits
//    for (int i = 0; i < m_port_size; ++i) {
//        // process the resolved requested port (only if flit is control flit)
//        if ((m_in_port[i].flit) && (m_in_port[i].is_flit_ctrl)) {
//            EV_DEBUG << "input port[" << i << "] requests"
//                     << " output port[" << m_in_port[i].requested_port <<
//                     "]\n";
//
//            // if no access is granted, set port state to discard
//            if (m_in_port[i].requested_port == DELETE) {
//                EV_DEBUG << TAGPUSH << "state[" << DELETE << "]"
//                         << "to in port[" << i << "]\n";
//                m_in_port[i].push_state(DELETE);
//
//            } else {
//                // if requested port is empty => claim it
//                if (m_out_port[m_in_port[i].requested_port].state.empty()) {
//                    EV_DEBUG << "Requested output-port is empty.\n";
//                    claimRequestedPort(i);
//
//                } else {
//                    int active_port =
//                        m_out_port[m_in_port[i].requested_port].top_state();
//                    int active_port_state =
//                    m_in_port[active_port].top_state();
//
//                    if (active_port == m_in_port[i].requested_port) {
//                        EV_DEBUG << "Active port[" << i
//                                 << "] is already owned.\n";
//                        claimRequestedPort(i);
//
//                    } else {
//                        // supplant all other cases
//
//                        // active port may have another ACK that already lost
//                        // against you (and therefore has a DELETE state)
//                        if (active_port_state == DELETE) {
//                            EV_DEBUG << "Active port state is delete and has "
//                                     << ((m_in_port[active_port].flit)
//                                             ? (util::convertMessageType(
//                                                   m_in_port[active_port]
//                                                       .flit->getKind()))
//                                             : ("no"))
//                                     << " flit\n";
//
//                        } else if (m_in_port[active_port].flit == nullptr) {
//                            EV_DEBUG << "Hit a hole.\n";
//                        }
//
//                        supplantPort(active_port, i);
//                    }
//                }
//            }
//        }
//    }
//}
//
// void CodedRouter::supplantPort(const int old_port, const int new_port)
//{
//    EV_DEBUG << "[supplant]"
//             << " active port[" << old_port << "]"
//             << " with port[" << new_port << "]\n";
//
//    emit(m_supplant_signal, true);
//
//    long msg_id = m_in_port[new_port].flit->getId();
//
//    claimRequestedPort(new_port);
//
//    // push delete to old port only if it differs from new port
//    if (old_port == new_port) {
//        EV_DEBUG << "[supplant: no push] supplanted port: "
//                 << "new port equals old port.\n";
//    } else {
//        // push new DELETE state to supplanted port
//        EV_DEBUG << TAGPUSH << "state[" << DELETE << "]"
//                 << " to in port[" << old_port << "]"
//                 << "\n";
//        m_in_port[old_port].push_state(DELETE, msg_id);
//    }
//
//    // store supplant information
//    m_in_port_supplant[new_port].supplant(old_port, msg_id);
//}
//
// void CodedRouter::releasePort(ResourceReleaseMsg *msg)
//{
//    FlagRouter::releasePort(msg);
//
//    int port_state = msg->getPort_state();
//    int port_id = msg->getPort_id();
//    int msg_id = msg->getMessage_id();
//
//    // finally, if an active connection was supplanted, remove your influence
//    on
//    // them as well
//    if (m_in_port_supplant[port_id].has_supplanted()) {
//        int supplanted_port = m_in_port_supplant[port_id].release(msg_id);
//        if (supplanted_port < 0) {
//            EV_DEBUG << TAGNOTEX << "msg<" << msg_id
//                     << "> in supplanted port structure\n";
//        } else {
//            if (supplanted_port == port_id) {
//                EV_DEBUG << "[supplant: no pop] supplanted port: "
//                         << "supplanted port equals current port.\n";
//            } else {
//                EV_DEBUG << TAGPOP << "state["
//                         << m_in_port[supplanted_port].top_state() << "]"
//                         << " from in port[" << supplanted_port << "]\n";
//                m_in_port[supplanted_port].pop_state();
//            }
//        }
//    }
//}
//
////
/////** Healing router implementation. */
//// Define_Module(SimpleHealingFlagRouter);
////
//// void SimpleHealingFlagRouter::childInitialize()
////{
////    m_fifo_size = par("fifo_size").longValue();
////
////    m_supplanted_port.resize(m_port_size);
////    m_supplanted_port_timestamp.resize(m_port_size);
////
////    m_port_fifo.resize(m_port_size);
////    for (int i = 0; i < m_port_size; ++i) {
////        m_port_fifo[i].resize(m_fifo_size);
////        // m_port_fifo[i] = new Fifo<RootFlit *>(m_fifo_size);
////    }
////}
////
//// void SimpleHealingFlagRouter::executeState(const int port, const int state)
////{
////    // if in_port state is discard, push the flit to the delay register
////    if (state == DELETE) {
////        if (m_port_fifo[port].full()) {
////            EV_DEBUG << "port[" << port << "] fifo full\n";
////            EV_DEBUG << TAGDEL << "flit<" <<
////            m_port_fifo[port].top()->getSequence_number() << "> from
/// fifo\n";
////            delete m_port_fifo[port].top();
////            m_port_fifo[port].pop();
////        }
////        EV_DEBUG << TAGPUSH << "flit<" <<
////        m_in_port[port].flit->getSequence_number() << "> to port[" << port
////                << "] fifo\n";
////        m_port_fifo[port].push(m_in_port[port].flit);
////
////        // otherwise forward the flits to the output registers
////    } else {
////        if (m_port_fifo[port].empty()) {
////            m_out_port[state].flit = m_in_port[port].flit;
////        } else {
////            m_out_port[state].flit = m_port_fifo[port].top();
////            m_port_fifo[port].pop();
////            m_port_fifo[port].push(m_in_port[port].flit);
////        }
////    }
////}
////
////
//// void CodedLengthRouter::releasePort(ResourceReleaseMsg *msg)
////{
////    int msg_id = msg->getMessage_id();
////    int port_state = msg->getPort_state();
////    int port_id = msg->getPort_id();
////
////    // remove own input port state
////    EV_DEBUG << TAGPOP << "state[" << port_state << "]"
////             << " pushed by msg<" << msg_id << ">"
////             << " from in port[" << port_id << "]\n";
////    m_in_port[port_id].pop_state();
////
////    if (m_in_port[port_id].pop_state(msg_id) < 0) {
////        EV_DEBUG << TAGNOTEX << "msg<" << msg_id << "> at port[" << port_id
////                 << "]\n";
////    }
////
////    // additionally remove claimed output port state, if the state is a
/// valid
////    // port (>= 0)
////    if (port_state > IDLE) {
////        if (m_out_port[port_state].pop_state(msg_id) < 0) {
////            EV_DEBUG << TAGNOTEX << "msg<" << msg_id << "> at port["
////                     << port_state << "]\n";
////        }
////    }
////
////    // finally, if an active connection was supplanted, remove your
/// influence
////    // on them as well
////    if (m_in_port_supplant[port_id].has_supplanted()) {
////        int supplanted_port = m_in_port_supplant[port_id].release(msg_id);
////        if (supplanted_port < 0) {
////            EV_DEBUG << TAGNOTEX << "msg<" << msg_id
////                     << "> in supplanted port structure\n";
////        } else {
////            if (m_in_port[supplanted_port].pop_state(msg_id) < 0) {
////                EV_DEBUG << TAGNOTEX << "msg<" << msg_id << ">"
////                         << " in port[" << supplanted_port << "]"
////                         << " that was supplanted\n";
////            }
////        }
////    }
////}
