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
// File      : router.h
// Created   : 03 May 2017 for apec_bless_noc
// Author(s) : Michael Vonbun
// E-mail(s) : michael.vonbun@tum.de
//

#ifndef __APEC_BLESS_NOC_ROUTER_H_
#define __APEC_BLESS_NOC_ROUTER_H_

// stl
#include <algorithm> // std::find
#include <deque>     // std::deque
#include <list>      // std::list
#include <stack>     // std::stack
#include <vector>    // std::vector

// omnet
#include <omnetpp.h>

// project utilities
#include <stl_fifo.h>

// project messages
#include <noc_flits.h>
#include <noc_messages.h>

// module

using namespace omnetpp;

/** Basic router.
 *
 *  The router has 1+ ports.
 *  Port[0] is the local port, the remaining ports connect to other routers.
 *
 *  The basic procedure is as follows.
 *  (1) all flits that arrive in one cycle are collected in an input buffer
 *  (2) arbitration is triggered as soon as at least one flit arrives
 *  (3) arbitration takes place
 *      (a) calculate desired output port
 *      (b) handle collisions
 *      (c) move flits from input to output buffer
 *      (d) delete flits that are not moved to the output buffer
 *  (4) transmission is triggered
 *      (a) if the router has a processing delay, the event should be the first
 * in the next cycle
 *      (b) if the router does not have a processing delay, the event should be
 * the last in the current cycle
 *      (c) can be avoided using send_delayed and using a late processed signal
 * to trigger sending
 *
 *
 *  Implementation details:
 *  We have to maintain an input port to output port mapping
 *
 * BaseRouter is Ack based
 */
class BaseRouter : public cSimpleModule
{
public:
    BaseRouter();
    virtual ~BaseRouter();

protected:
    /** @name Router parameters. */
    //@{
    int m_id;                   ///< Module id.
    simtime_t m_t_clk;          ///< Clock period.
    int m_num_rows;             ///< Number of rows in 2D-mesh network.
    int m_num_columns;          ///< Number of columns in 2D-mesh network.
    int m_row;                  ///< Own row.
    int m_column;               ///< Own column.
    int m_num_nodes;            ///< Number of network nodes.
    int m_delay;                ///< Router delay in cycles.
    int m_num_states_threshold; ///< Max number of states expected.
    //@}

    /** @name Base ports. */
    //@{
    int m_in_port_base_id;                    ///< Base id of ports.
    int m_out_port_base_id;                   ///< Base id of out-port.
    int m_port_size;                          ///< Port size.
    int m_inport_size;                        ///< Inport size.
    int m_current_outport_request_processing; ///< Utility.

    struct Base_Port_t {
        cGate *gate;    ///< Alias of port gate.
        RootFlit *flit; ///< Flit register.
        Base_Port_t() : gate(nullptr), flit(nullptr){};
        virtual void reset() { flit = nullptr; };
        virtual void storeFlit(RootFlit *new_flit) { flit = new_flit; };
    };
    //@}

    /** @name Input ports (stateful). */
    //@{
    struct Input_Port_t : Base_Port_t {
        Input_Port_t() : Base_Port_t()
        {
            reset();
            request.push_back(IDLE);
            timestamp.push_back(simTime());
        };
        virtual void reset()
        {
            Base_Port_t::reset();
            is_flit_head = false;
            is_flit_tail = false;
            is_flit_ack = false;
            is_flit_nack = false;
            is_flit_cnack = false;
            is_flit_ctrl = false;
            is_flit_release = false;
            release_triggered = false;
            state = NOREQUEST;
        };

        /// Flit type indicators.
        bool is_flit_head;
        bool is_flit_tail;
        bool is_flit_ack;
        bool is_flit_nack;
        bool is_flit_cnack;
        bool is_flit_ctrl;
        bool is_flit_release;
        bool release_triggered;
        virtual void storeFlit(RootFlit *new_flit)
        {
            Base_Port_t::storeFlit(new_flit);
            is_flit_head = util::isHead(new_flit);
            is_flit_tail = util::isTail(new_flit, true);
            is_flit_ack = util::isAck(new_flit);
            is_flit_nack = util::isNack(new_flit);
            is_flit_cnack = util::isCNack(new_flit);
            is_flit_ctrl = util::hasControlInfo(new_flit);
            is_flit_release = util::releasesPort(new_flit);
            release_triggered = false;
        };
        bool isRequestCurrent(const int port_id)
        {
            return (port_id == request.back());
        };
        bool isRequestCtrl(const int port_id)
        {
            return ((is_flit_ctrl) && isRequestCurrent(port_id));
        };
        int getRequestPriority(const int port_id)
        {
            for (size_t i = 0; i < request.size(); ++i) {
                if (getRequest(i) == port_id) {
                    return getPriority(i);
                }
            }
        };

        /// Port action / state.
        int state;
        void grantRequest(const int port_id)
        {
            if (isRequestCurrent(port_id)) {
                grantRequest();
            }
        };
        void grantRequest() { state = getRequest(); };
        void rejectRequest(const int port_id)
        {
            if (isRequestCurrent(port_id)) {
                rejectRequest();
            }
        };
        void rejectRequest() { state = DELETE; };

        /// Packet action.
        /// Increase prio of granted packet.
        void grantPacket() { --priority.back(); };
        /// Set requests for remaining flits to DELETE for rejected packet.
        void rejectPacket() { request.back() = DELETE; };

        /// Port connection state LIFO.
        std::deque<int> request;     ///< Port request.
        std::deque<int> priority;    ///< Request priority.
        std::deque<long> message_id; ///< Id of message who pushed the state.
        std::deque<simtime_t> timestamp; ///< Timestamp of push.

        /// Return request data.
        /// 0 = current, 1 = previous, ..., (size-1) = first (idle)
        int getRequest(const unsigned req = 0)
        {
            size_t sz = request.size();
            assert(req < (sz - 1)); // ensure no access on first (idle) request

            return request[sz - 1 - req];
        };
        simtime_t getTimestamp(const unsigned req = 0)
        {
            size_t sz = timestamp.size();
            assert(req < (sz - 1)); // ensure no access on first (idle) request

            return timestamp[sz - 1 - req];
        };
        /// 0 = current, 1 = previous, ..., (size-1) = first (no idle pushed,
        /// see constructor)
        int getPriority(const unsigned req = 0)
        {
            size_t sz = priority.size();
            assert(req < sz); // ensure no access on first (idle) request

            return priority[sz - 1 - req];
        };
        long getMessageId(const unsigned req = 0)
        {
            size_t sz = message_id.size();
            assert(req < sz); // ensure no access on first (idle) request

            return message_id[sz - 1 - req];
        };

        /// Push, Pop, and modify top.
        void pushRequest(int request_val, int prio_val = PRIO_PACKET_BASE,
                         long msg_id = -1)
        {
            if (msg_id == -1)
                (msg_id = flit->getId());
            request.push_back(request_val);
            priority.push_back(prio_val);
            message_id.push_back(msg_id);
            timestamp.push_back(simTime());
        };
        void popRequest()
        {
            request.pop_back();
            priority.pop_back();
            message_id.pop_back();
            timestamp.pop_back();
        };
        int popRequest(long msg_id)
        {
            // find index of msg_id
            int index = -1;
            for (std::deque<long>::iterator it_message_id = message_id.begin();
                 it_message_id != message_id.end(); ++it_message_id) {
                if (*it_message_id == msg_id) {
                    index = it_message_id - message_id.begin();

                    // and erase the content at that index
                    request.erase(request.begin() + index);
                    priority.erase(priority.begin() + index);
                    timestamp.erase(timestamp.begin() + index);
                    message_id.erase(it_message_id);
                    break;
                }
            }
            return index;
        };
        void update(int state_val, int prio_val = PRIO_PACKET_BASE,
                    long msg_id = -1)
        {
            if (msg_id == -1)
                (msg_id = flit->getId());
            request.back() = state_val;
            priority.back() = prio_val;
            message_id.back() = msg_id;
            timestamp.back() = simTime();
        };
    };
    std::vector<Input_Port_t> m_in_port; ///< Input ports.
    //@}

    /** @name Output ports (stateful). */
    //@{
    struct Output_Port_t : Base_Port_t {
        std::list<int> requested_by;
        std::list<int>::iterator it_requested_by;

        std::deque<int> connection;
        std::deque<long>
            message_id; ///< Id of message who pushed the connection.

        Output_Port_t() : Base_Port_t() { reset(); };
        virtual void reset()
        {
            Base_Port_t::reset();
            requested_by.clear();
        };
        // Add requesting port's id only if it is not already in the list
        void addRequestingPort(const int port_id)
        {
            if (std::find(requested_by.begin(), requested_by.end(), port_id) ==
                requested_by.end()) {
                requested_by.push_back(port_id);
            }
        };
        void pushConnection(int port, long msg_id = -1)
        {
            if (msg_id == -1)
                (msg_id = flit->getId());
            connection.push_back(port);
            message_id.push_back(msg_id);
        };
        int popConnection()
        {
            connection.pop_back();
            message_id.pop_back();
        };
        int popConnection(long msg_id)
        {
            // find index of msg_id
            int index = -1;
            for (std::deque<long>::iterator it_message_id = message_id.begin();
                 it_message_id != message_id.end(); ++it_message_id) {
                if (*it_message_id == msg_id) {
                    index = it_message_id - message_id.begin();

                    // and erase the content at that index
                    connection.erase(connection.begin() + index);
                    message_id.erase(it_message_id);
                    break;
                }
            }
            return index;
        };
        bool isIdle() { return connection.empty(); };
    };
    std::vector<Output_Port_t> m_out_port; ///< Output ports.
    //@}

    bool m_ctrl_flit_arrived; ///< Control flit arrival indicator.

    /** @name Self messages / events. */
    //@{
    cMessage *m_arbitration_event;
    cMessage *m_tx_event;
    //@}

protected:
    /** @name Omnet interface. */
    //@{
    virtual void initialize() override;
    virtual void finish() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void childInitialize() = 0; ///< Inits child members.
    virtual void nackInitialize(){};    ///< Inits for nack based router;
    //@}

    /** @name Get router kind. */
    //@{
    /// Nack or Ack based?
    virtual bool isNackRouter() { return false; };

    /// Release with tail or length?
    virtual bool isFlagRouter() = 0;

    /// Coded or plain router?
    virtual bool isCodedRouter() = 0;
    //@}

    /** @name First stage (collection). */
    //@{
    /// Store @param flit in port @param input_id's register.
    virtual void storeFlit(RootFlit *flit, const int inport_id);

    /// Process control flit at port @param inport_id
    void processControlInfoAtPort(const int inport_id);

    /// Interface: push request and priority at port @param inport_id.
    virtual void pushRequestAtPort(const int inport_id) = 0;

    /// ACKs & HEADs only request ports.
    int getOutPortFromCtrlFlitAtPort(const int inport_id);
    //@}

    /** @name Second stage (arbitration). */
    //@{
    void triggerArbitration(); ///< Trigger arbitration in same cycle.
    void arbitrate();          ///< Router's main.

    void collectPortRequests(); ///< Collect all requests from input ports.
    void processPortRequests(); ///< Process all requests from input ports.

    /// Interface: sort the requests for output port @param output_id.
    virtual void sortPortRequests(const int outport_id) = 0;

    /// Grant and reject the requests.
    void grantRequest(const int inport_id);
    void rejectRequest(const int inport_id);

    /// Set states of input ports requesting output port @param outport_id
    /// based on the sort result.
    virtual void setPortStates(const int outport_id);

    /// Compare function for all flits. Priority is active < new packet.
    bool compareRequests(const int &first, const int &second);

    /// Compare function for control flits only.
    virtual bool compareCtrlRequests(const int &first, const int &second);

    /// Compare function using request priorities.
    bool compareRequestPriorities(const int &first, const int &second);

    /// Compare function for non-control flits.
    virtual bool compareRegularRequests(const int &first,
                                        const int &second) = 0;

    /// Execute the port states.
    virtual void executePortStates();

    /// Interface: execute @param state on @param port.
    virtual void executeState(const int port, const int state) = 0;

    /// Interface: release ports.
    virtual void triggerPortReleases() = 0;

    /// Release @param which port.
    virtual void releasePort(ResourceReleaseMsg *msg) = 0;
    //@}

    /** @name Third stage (transmission). */
    //@{
    void triggerTransmission(); ///< Trigger transmission.
    void transmit(); ///< Send delayed all flits in the output registers.
    void resetRouterStates(); ///< Reset all router states for next cycle.
    //@}

    /** Utilities. */
    //@{
    void printPortStates();
    void printPortState(const int port_id,
                        const bool port_type_is_input = true);
    virtual void validatePortStates() = 0;
    //@}

    /** @name State enumerations. */
    //@{
    /// Router port states / actions.
    /// Non-negative states <S> are used to indicate a connection to port <S>.
    /// Use only negative states here and best check for equality.
    enum RouterStates {
        IDLE = -1,      // port is idling
        NOREQUEST = -2, // state of port request
        DELETE = -3,    // delete incoming flits util next head or tail arrives
        REQUESTDENIED = -4, //
        SUPPLANT = -5,      //
        STORE = -6          //
    };
    //@}

    /** @name Scheduling priority enumerations. */
    //@{
    enum RouterSchedPriorities {
        SP_ARBITRATE = SP_LATE,
        SP_RELEASE,
        SP_TRANSMIT
    };
    //@}

    /** @name Statistics recording. */
    //@{
    simsignal_t m_flit_drop_signal;            ///< Record every flit deletion.
    simsignal_t m_ack_drop_signal;             ///< Record only ack deletion.
    simsignal_t m_packet_drop_signal;          ///< Record packet deletion.
    simsignal_t m_packet_drop_distance_signal; ///< Record distance traveled
                                               /// before deletion.
    simsignal_t m_nack_reroute_signal; ///< Record number of rerouted nacks.
    simsignal_t m_cnack_number_signal; ///< Record number of nacks in cnack.

    /// record statistics of a packet drop
    void recordPacketDrop(RootFlit *flit);
    void recordPacketDrop(HeadFlit *head);
    //@}

protected:
    bool m_are_input_ports_reset;
};

/** Plain router without intersecting flits. */
class PlainRouter : public virtual BaseRouter
{
protected:
    /// Second stage initialization.
    virtual void childInitialize() override;

    virtual bool isCodedRouter() override { return false; };

    virtual bool compareRegularRequests(const int &first,
                                        const int &second) override;

    /** @name Parent router interface. */
    //@{
    virtual void pushRequestAtPort(const int inport_id) override;
    virtual void sortPortRequests(const int outport_id) override;
    virtual void executeState(const int port, const int state) override;
    virtual void releasePort(ResourceReleaseMsg *msg) override;
    //@}
};

/** Flag Router uses tail flags to release resources.
 *
 */
class FlagRouter : public virtual BaseRouter
{
protected:
    virtual bool isFlagRouter() override { return true; };
    /** @name Parent router interface. */
    //@{
    virtual void triggerPortReleases() override;
    virtual void validatePortStates() override;
    //@}
};

/** Length router uses packet length to release resources.
 *
 */
class LengthRouter : public virtual FlagRouter
{
protected:
    virtual bool isFlagRouter() override { return false; };
    /** @name Parent router interface. */
    //@{
    virtual void triggerPortReleases() override;
    //@}
};

/** Router which allows multiple active connections.
 *
 */
class CodedRouter : public virtual PlainRouter
{
protected:
    virtual bool isCodedRouter() override { return true; };

    /** @name Router parameters. */
    //@{
    bool m_supplant_acks_only;
    int m_supplant_packet_length_threshold;
    //@}

    /** @name Router members. */
    //@{
    simsignal_t m_supplant_signal; ///< Statics signal to count supplants.
    //@}

    /// Second stage initialization.
    virtual void childInitialize() override;

    /** @name Parent router interface. */
    //@{
    virtual bool compareRegularRequests(const int &first,
                                        const int &second) override;
    //    virtual void validatePortStates() override;
    //    virtual void setPortStates() override;
    //    virtual void releasePort(ResourceReleaseMsg *msg) override;
    //    //@}
    //
    //    /** @name Router specific methods. */
    //    //@{
    //    void supplantPort(const int old_port, const int new_port);
    //
    //    struct Supplant_s {
    //        std::deque<int> port;
    //        std::deque<simtime_t> timestamp;
    //        std::deque<long> message_id;
    //
    //        void supplant(const int old_port, const long msg_id)
    //        {
    //            port.push_back(old_port);
    //            timestamp.push_back(simTime());
    //            message_id.push_back(msg_id);
    //        };
    //
    //        int release(const long msg_id)
    //        {
    //            int supplanted_port = -1;
    //            for (std::deque<long>::iterator it_message_id =
    //            message_id.begin();
    //                 it_message_id != message_id.end(); ++it_message_id) {
    //                if (*it_message_id == msg_id) {
    //                    int index = it_message_id - message_id.begin();
    //
    //                    // return found port
    //                    supplanted_port = port[index];
    //
    //                    // and erase the content at that index
    //                    port.erase(port.begin() + index);
    //                    timestamp.erase(timestamp.begin() + index);
    //                    message_id.erase(it_message_id);
    //                    break;
    //                }
    //            }
    //            return supplanted_port;
    //        };
    //
    //        bool has_supplanted() { return !port.empty(); };
    //    };
    //
    //    /// Per port supplant management struct.
    //    std::vector<Supplant_s> m_in_port_supplant;

    //@}
};

/** Nack Router generates NACK if Head flit is rejected.
 *
 */
class NackRouter : public virtual BaseRouter
{
public:
    virtual ~NackRouter();

protected:
    virtual void nackInitialize() override;
    virtual bool isNackRouter() override { return true; };

    virtual void storeFlit(RootFlit *flit, const int inport_id) override;
    int getOutPortForNack(NackFlit *nack);
    CNackFlit *genCNack(RootFlit *first, RootFlit *second);

    void genNack(HeadFlit *head, int head_inport_id);
    void genCNackCollided(const int first, const int second);
    virtual void executePortStates() override;
    void storeNackFlit(NackFlit *nack, const int inport_id);

    // same as Base, but if nack is second reroute!
    virtual void setPortStates(const int outport_id) override;
    void reroute(const int inport_id);

    virtual bool compareCtrlRequests(const int &first,
                                     const int &second) override;
};

/** Ack based tail release router.
 *
 */
class PlainFlagAckRouter : public FlagRouter, public PlainRouter
{
};

/** Nack based tail release router.
 *
 */
class PlainFlagNackRouter : public NackRouter,
                            public FlagRouter,
                            public PlainRouter
{
protected:
    virtual bool compareRegularRequests(const int &first,
                                        const int &second) override;
};

/** Ack based length release router.
 *
 */
class PlainLengthAckRouter : public LengthRouter, public PlainRouter
{
};

/** Nack based length release router.
 *
 */
class PlainLengthNackRouter : public NackRouter,
                              public LengthRouter,
                              public PlainRouter
{
protected:
    virtual bool compareRegularRequests(const int &first,
                                        const int &second) override;
};

/** Ack based flag release router (coded).
 *
 */
class CodedFlagAckRouter : public CodedRouter, public FlagRouter
{
};

/** Nack based tail release router (coded).
 *
 */
class CodedFlagNackRouter : public NackRouter,
                            public FlagRouter,
                            public CodedRouter
{
protected:
    virtual bool compareRegularRequests(const int &first,
                                        const int &second) override;
};

/** Ack based length release router (coded).
 *
 */
class CodedLengthAckRouter : public CodedRouter, public LengthRouter
{
};

/** Nack based length release router (coded).
 *
 */
class CodedLengthNackRouter : public NackRouter,
                              public LengthRouter,
                              public CodedRouter
{
protected:
    virtual bool compareRegularRequests(const int &first,
                                        const int &second) override;
};

///** Router which allows multiple active connections but buffers flits.
// *  Uses tail flags to release resources.
// */
// class SimpleHealingFlagRouter : public SimpleCodedFlagRouter
//{
// protected:
//    virtual void childInitialize() override;
//    virtual void executeState(const int port, const int state) override;
//
//    std::size_t m_fifo_size;
//    std::vector<std::stack<int>> m_supplanted_port;
//    std::vector<std::stack<simtime_t>> m_supplanted_port_timestamp;
//    std::vector<Fifo<RootFlit *>> m_port_fifo;
//};
//
///** Plain router without intersecting flits.
// *  Uses packet length to release resources.
// *  TODO
// */
// class LengthRouter : public BaseRouter
//{
//};
//
///** Router which allows multiple active connections.
// *  Uses packet length to release resources.
// *  TODO
// */
// class SimpleCodedLengthRouter : public LengthRouter
//{
//};
//
///** Router which allows multiple active connections but buffers flits.
// *  Uses packet length to release resources.
// *  TODO
// */
// class SimpleHealingLengthRouter : public SimpleCodedLengthRouter
//{
//};

#endif
