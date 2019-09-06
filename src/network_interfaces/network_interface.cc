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
// File      : network_interface.cc
// Created   : 11 Apr 2017 for apec_bless_noc
// Author(s) : Michael Vonbun
// E-mail(s) : michael.vonbun@tum.de
// 

// stl
#include <cassert> // assert
#include <cstdio>  // printf

// project
#include <add_module_statistic.h>
#include <enum_utilities.h> // util::isHead, util::isTail
#include <noc_flits.h>      // BaseFlit, HeadFlit, AckFlit
#include <noc_messages.h>   // TimeoutMsg
#include <noc_packet.h>     // NocPacket

// header
#include <network_interface.h>

// Define_Module(NetworkInterface);

NetworkInterface::NetworkInterface()
    : m_new_packet{nullptr}, m_transmit_event{nullptr}, m_timeout{nullptr},
      m_tx_flit{nullptr}
{
}

NetworkInterface::~NetworkInterface()
{
    EV_DEBUG << "[DESTRUCTOR] ni[" << m_id << "]"
             << "\n";
    cancelAndDelete(m_transmit_event);

    // pending buffer
    for (size_t i = 0; i < m_pending_buffer.size(); ++i) {
        cancelEvent(m_pending_buffer.getTimeoutMessage(i, true));
    }
    m_pending_buffer.eraseAndDeleteAll();

    // packet collector
    if (m_new_packet) {
        delete m_new_packet;
    }
}

void NetworkInterface::initialize()
{
    // get static module parameters from ned
    m_id = par("id").longValue();
    m_t_clk = par("t_clk");
    m_max_num_pending_packets = par("max_pending_packets");
    m_late_ack_compensation_enabled = par("late_ack_compensation").boolValue();
    m_end_simulation_on_full_buffer =
        par("end_simulation_on_full_buffer").boolValue();

    // reset members
    m_transmit_event = new cMessage("event");
    m_transmit_event->setKind(TXEVENT);
    m_transmit_event->setSchedulingPriority(SP_VERY_LATE); // TODO CHECK PRIO
    m_packet_in_transmission_id = -1;

    // register statistic signals
    // clang-format off
    // retransmission buffer
    m_pending_buffer_num_packets = util::addModuleStatistic(
        this, "ni-", m_id, "_pendingBuf_num_packet", "ni_pendingBuf_num_packet");
    m_pending_buffer_num_flits = util::addModuleStatistic(
        this, "ni-", m_id, "_pendingBuf_num_flit", "ni_pendingBuf_num_flit");

    // output queues
    m_output_queue_num_packet_flits = util::addModuleStatistic(
        this, "ni-", m_id, "_outputQ_num_packet_flit", "ni_outputQ_num_packet_flit");
    m_output_queue_num_acks = util::addModuleStatistic(
        this, "ni-", m_id, "_outputQ_num_acks", "ni_outputQ_num_acks");

    // ack based
    m_ack_packet_attempts = util::addModuleStatistic(
        this, "ni-", m_id, "_packet_attempts", "ni_packet_attempts");
    m_ack_packet_latency_generation = util::addModuleStatistic(
        this, "ni-", m_id, "_packet_latency_generation_cycles", "ni_packet_latency_generation_cycles");
    m_ack_packet_latency_injection = util::addModuleStatistic(
        this, "ni-", m_id, "_packet_latency_injection_cycles", "ni_packet_latency_injection_cycles");
    m_num_late_acks = util::addModuleStatistic(
            this, "ni-", m_id, "_num_late_acks", "ni_num_late_acks");
    m_ack_latency_generation = util::addModuleStatistic(
        this, "ni-", m_id, "_ack_latency_generation_cycles", "ni_ack_latency_generation_cycles");
    m_ack_latency_injection = util::addModuleStatistic(
        this, "ni-", m_id, "_ack_latency_injection_cycles", "ni_ack_latency_injection_cycles");
    m_ack_latency_network_access = util::addModuleStatistic(
            this, "ni-", m_id, "_ack_latency_network_access_cycles", "ni_ack_latency_network_access_cycles");

    // nack based
    m_nack_latency_generation = util::addModuleStatistic(
        this, "ni-", m_id, "_nack_latency_generation_cycles", "ni_nack_latency_generation_cycles");

    // send based
    m_num_flit_injected = util::addModuleStatistic(
        this, "ni-", m_id, "_num_flits_injected", "ni_num_flits_injected");
    m_num_ack_injected = util::addModuleStatistic(
            this, "ni-", m_id, "_num_ack_injected", "ni_num_ack_injected");
    m_num_packet_injected = util::addModuleStatistic(
            this, "ni-", m_id, "_num_packet_injected", "ni_num_packet_injected");

    // new packet based
    m_num_flit_generation_rate_payload = util::addModuleStatistic(
        this, "ni-", m_id, "_flit_generation_rate_payload", "ni_flit_generation_rate_payload");
    m_num_flit_generation_rate = util::addModuleStatistic(
        this, "ni-", m_id, "_flit_generation_rate", "ni_flit_generation_rate");
    // clang-format on

    // add gui watches
    //    WATCH_VECTOR(m_packet_collector);
    //    WATCH_OBJ(m_packet_buffer);
    //    WATCH_PTR(m_new_head);
    //    WATCH_PTR(m_tx_flit);
}

void NetworkInterface::finish()
{
#ifdef MODULEFINISHPRINTF
    EV_DEBUG << "pending packets: " << m_pending_buffer.size() << "\n";
    getOutputBuffer()->printStatus();
#endif
    recordScalar("pending_packets", m_pending_buffer.size());
}

void NetworkInterface::handleMessage(cMessage *msg)
{
// add validation code here
#ifdef VALIDATE
    if (!msg->isSelfMessage()) {
        std::cout << "t=" << simTime();
        printf("\t%10s%-10s\t kind=%2d\t msgid=%-8ld",
               this->getParentModule()->getFullName(), ".ni", msg->getKind(),
               msg->getId());
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

    if (msg->arrivedOn("source")) {
        // new (outgoing) packet
        handleNewPacket(msg);

    } else if (msg->arrivedOn("sink$i")) {
        // new outgoing acks
        handleNewAck(msg);
    } else if (msg->isSelfMessage()) {
        // events
        if (msg->getKind() == TXEVENT) {
            popAndSend();

        } else if (msg->getKind() == TIMEOUT) {
            handleTimeout(check_and_cast<TimeoutMsg *>(msg));
        }

    } else if (msg->arrivedOn("port$i")) {
        // incoming messages
        handleIncomingFlits(msg);
    }
}

/// Handle new packets from the source.
///
/// Distribute the messages incoming from source port to its
/// respective subroutines.
///
/// All flits of a packet are first collected in a vector
/// and pushed to the pending packets buffer afterwards.
void NetworkInterface::handleNewPacket(cMessage *msg)
{
    if (util::isHead(msg)) {
        handleNewHead(check_and_cast<HeadFlit *>(msg));
    }

    handleNewFlit(msg);

    if (isNewLast(msg)) {
        handleNewLastflit();
    }
}

/// Handle the start of a new packet.
void NetworkInterface::handleNewHead(HeadFlit *head)
{
    EV_INFO << TAGRX << "head of new " << PACKET(head) << "\n";
    EV_DEBUG << TAGNEW << "NocPacket\n";
    m_new_packet = new NocPacket; // create new packet container
}

/// Common operation to all flits of new packet.
void NetworkInterface::handleNewFlit(cMessage *flit)
{
    m_new_packet->push_back(flit); // insert flit into new packet
    EV_DEBUG << TAGPUSH << "flit<" << (m_new_packet->id()) << ":";
    EV_DEBUG << ((m_new_packet->head())->getNum_flit()) << "::";
    EV_DEBUG << (m_new_packet->size() - 1) << ">";
    EV_DEBUG << " to packet collector\n";
}

/// Conclude a new packet.
void NetworkInterface::handleNewLastflit()
{
    // check if pending buffer is full
    // this way, we lower the generation rate, as we delete packets almost
    // immediately after creation
    if (m_pending_buffer.size() < m_max_num_pending_packets) {
        // a new timeout is created upon insertion
        m_pending_buffer.insert(m_new_packet);

        // count the number of packets / flits currently in the pending buffer
        emit(m_pending_buffer_num_packets, m_pending_buffer.size());
        emit(m_pending_buffer_num_flits, m_pending_buffer.getNumFlit());
        emit(m_num_flit_generation_rate_payload,
             m_new_packet->head()->getNum_baseflit());
        emit(m_num_flit_generation_rate, m_new_packet->head()->getNum_flit());

        // enqueue packet in output buffer
        getOutputBuffer()->duplicateAndPush(*m_new_packet);
        triggerTransmission(m_t_clk);

    } else {

        EV_DEBUG << "Pending buffer full. Deleting packet<"
                 << m_new_packet->head()->getPacket_id() << ">\n";

        delete m_new_packet;

        // if realistic buffer behavior is intended, terminate the simulation
        // once no new packet can be inserted in the pending buffer anymore
        if (m_end_simulation_on_full_buffer) {
            // emit simulation termination message to own tile's sink
            cModule *targetModule = getParentModule()->getSubmodule("sink");
            sendDirect(new PBuffOverflowMsg(m_id), targetModule,
                       "abort_simulation_port");
        }
    }

    // invalidate temporary buffer (used such that the destructor does not
    // attempt to delete the same object twice)
    m_new_packet = nullptr;
}

/// Handle flits from the network.
///
/// Distribute the messages incoming from the network port to its
/// respective subroutines.
void NetworkInterface::handleIncomingFlits(cMessage *msg)
{
    // acks are treated specially
    if (msg->getKind() == ACK) {
        handleIncomingAck(check_and_cast<AckFlit *>(msg));
    } else {
        if (msg->getKind() == NACK) {
            handleIncomingNack(check_and_cast<NackFlit *>(msg));
        } else {
            if (msg->getKind() == CNACK) {
                handleIncomingCNack(check_and_cast<CNackFlit *>(msg));
            } else {
                handleIncomingOther(msg);
            }
        }
    }
}

/// Handle all non-ack flits from network (forward).
void NetworkInterface::handleIncomingOther(cMessage *msg)
{
    EV_INFO << TAGTX << "forward flit to decoder\n";
    send(msg, "sink$o");
}

/// Handle ack from decoder to network.
void NetworkInterface::handleNewAck(cMessage *msg)
{
    bool was_empty = getOutputBuffer()->push(msg);
    if (was_empty) {
        triggerTransmission(m_t_clk);
    }
}

/// Schedule the timeout for an outgoing packet with id.
void NetworkInterface::scheduleTimeout(const long id)
{
    EV_INFO << TAGBEG << "timer ";
    EV_INFO << "for packet<" << id << ">\n";

    m_timeout = m_pending_buffer.getTimeout(id);
    if (m_timeout != nullptr) {
        EV_DEBUG << "timeout firing after <"
                 << m_timeout->getTimeout() / m_t_clk << "> cycles\n";
        if (m_timeout->isScheduled())
            cancelEvent(m_timeout);
        scheduleAt(simTime() + m_timeout->getTimeout(), m_timeout);
    } else {
        EV_DEBUG << TAGNOTEX << "packet<" << id << "> in pending buffer\n";
        EV_DEBUG << "No timeout started\n";
    }
}

/// Schedule transmission event.
void NetworkInterface::triggerTransmission(const simtime_t time)
{
    if (getOutputBuffer()->empty()) {
        EV_DEBUG << TAGTRIG
                 << "transmission not possible, output buffer is empty\n";
    } else if (m_transmit_event->isScheduled()) {
        EV_DEBUG << "transmission is already scheduled\n";
    } else {
        EV_INFO << TAGTRIG << "new transmission\n";
        scheduleAt(simTime() + time, m_transmit_event);
    }
}

/// Pop flit from output buffer and inject it into the network.
void NetworkInterface::popAndSend()
{
    bool last = false;
    m_tx_flit = (getOutputBuffer()->pop(&last));
    txCheckAndHandleHead();
    txCheckAndHandleAck();
    txCheckAndHandleNack();

    if (m_tx_flit != nullptr) {
        txCheckAndHandleLast(last);

        send(m_tx_flit, "port$o");       // send out
        emit(m_num_flit_injected, true); // record injection rate

        emit(m_num_ack_injected, util::isAck(m_tx_flit));
        if (util::isHead(m_tx_flit)) {
            emit(m_num_packet_injected,
                 check_and_cast<HeadFlit *>(m_tx_flit)->getNum_flit());
        }

        triggerTransmission(m_t_clk); // trigger new transmission after t_clk

    } else {
        EV_DEBUG << TAGDBG << "no more packets in the output buffer\n";
    }

    // store the number of flits in the packet buffer and in the ack buffer
    emit(m_output_queue_num_packet_flits, getOutputBuffer()->packetQueueSize());
    emit(m_output_queue_num_acks, getOutputBuffer()->ackQueueSize());

    // print the output queue status (number of packet flits and acks)
    getOutputBuffer()->printStatus();
}

/// Handle outgoing ack flit.
void NetworkInterface::txCheckAndHandleAck()
{
    if ((m_tx_flit != nullptr) && (util::isAck(m_tx_flit))) {
        AckFlit *ack = check_and_cast<AckFlit *>(m_tx_flit);
        ack->setInjection_time(simTime());
    }
}

/// Handle outgoing nack flit.
void NetworkInterface::txCheckAndHandleNack()
{
    if ((m_tx_flit != nullptr) && (util::isNack(m_tx_flit))) {
        NackFlit *nack = check_and_cast<NackFlit *>(m_tx_flit);

        // check if nack is generated by the module or if it is a deflected nack
        // (can only happen in flag mode)
        int source_id = nack->getSource();
        if (source_id == m_id) {
            nack->setInjection_time(simTime());
        }
    }
}

/// Handle outgoing packet's head flit.
void NetworkInterface::txCheckAndHandleHead()
{
    if ((m_tx_flit != nullptr) && (util::isHead(m_tx_flit))) {
        HeadFlit *head = check_and_cast<HeadFlit *>(m_tx_flit);
        m_packet_in_transmission_id = head->getPacket_id();
        NocPacket *packet =
            m_pending_buffer.getPacket(m_packet_in_transmission_id, false);

        if (packet != nullptr) {
            // if packet is injected the first time, set the injection time
            // subsequent head flits will get the injection time from the dup()
            // of the pending buffer
            if (head->getAttempts() < 1) {
                packet->head()->setInjection_time(simTime());
                head->setInjection_time(packet->head()->getInjection_time());
            }

            // schedule the timeout
            scheduleTimeout(m_packet_in_transmission_id);
        } else {
            assert(false &&
                   "ACK arrived after the retransmission timeout expired");
            txCheckAndHandleHeadChild(head);
        }
    }
}

/// Handle outgoing packet's last flit.
void NetworkInterface::txCheckAndHandleLast(bool last)
{
    if (txIsLast() || last) {
        // The last flit of the current packet is out
        m_packet_in_transmission_id = -1; // reset id of currently active packet
    }
}

/// Check if transmission condition is met and retrigger transmission
// void NetworkInterface::checkTransmissionCondition()
//{
//    if ((!m_ack_buffer.isEmpty()) || (!m_packet_buffer.isEmpty())) {
//        EV_INFO << TAGTRIG << "new transmission\n";
//        triggerTransmission(m_t_clk);
//    }
//}

// Nack and Ack based Network Interface
// delete paket in pending buffer
void AckNetworkInterface::handleIncomingAck(AckFlit *ack)
{
    EV_INFO << TAGRX << "ack<";
    EV_INFO << ack->getPacket_id() << ">\n";
    m_timeout = m_pending_buffer.getTimeout(ack);
    if (m_timeout != nullptr) {
        cancelEvent(m_timeout); // cancel event
        // delete pending packet (timeout and packet)
        m_pending_buffer.eraseAndDelete(ack);

        // record acked packet statistics
        // - attempts
        // - time between packet generation / injection and ack reception
        simtime_t delay_generation =
            (ack->getArrivalTime() - ack->getPacket_generation_time());
        simtime_t delay_injection =
            (ack->getArrivalTime() - ack->getPacket_injection_time());
        emit(m_ack_packet_attempts, ack->getPacket_attempts());
        emit(m_ack_packet_latency_generation,
             (int)(delay_generation / m_t_clk));
        emit(m_ack_packet_latency_injection, (int)(delay_injection / m_t_clk));

        // record ack statistics
        delay_generation = (ack->getArrivalTime() - ack->getGeneration_time());
        delay_injection = (ack->getArrivalTime() - ack->getInjection_time());
        simtime_t delay_network_access =
            (ack->getInjection_time() - ack->getGeneration_time());
        emit(m_ack_latency_generation, (int)(delay_generation / m_t_clk));
        emit(m_ack_latency_injection, (int)(delay_injection / m_t_clk));
        emit(m_ack_latency_network_access,
             (int)(delay_network_access / m_t_clk));
    }
    delete ack; // delete ack itself
}

// Should not happen
void AckNetworkInterface::handleIncomingNack(NackFlit *nack)
{
    // Nack should no get received in Ack based Network
    delete nack;
    assert(false);
}

// Should not happen
void AckNetworkInterface::handleIncomingCNack(CNackFlit *cnack)
{
    // cNack should no get received in Ack based Network
    delete cnack;
    assert(false);
}

// Retransmit packet after Timeout
void AckNetworkInterface::handleTimeout(TimeoutMsg *msg)
{
    long packet_id = msg->getPacket_id();

    EV_INFO << TAGTRIG << "timeout ";
    EV_INFO << "for packet<" << packet_id << ">\n";

    NocPacket *packet = m_pending_buffer.getPacket(packet_id, true);
    if (packet != nullptr) {
        bool was_empty = getOutputBuffer()->duplicateAndPush(*packet);
        if (was_empty) {
            triggerTransmission(m_t_clk); // trigger transmission immediately
        }
    } else {
        EV_DEBUG << TAGNOTEX << "packet<" << packet_id
                 << "> in pending buffer\n";
    }
}

/// Handle outgoing head flit.
/// Might delete entire packet and change m_tx_flit to point to the next packet.
void AckNetworkInterface::txCheckAndHandleHeadChild(HeadFlit *head)
{
    if (m_late_ack_compensation_enabled) {
        // ack arrived late, so packet is removed from pending buffer but
        // still in output queue
        // remove packet from output queue and record the incident
        EV_DEBUG << "Attempt to inject head of packet["
                 << m_packet_in_transmission_id
                 << "] failed as it was removed from the pending buffer"
                 << "\n";
        emit(m_num_late_acks, head->getNum_flit());
        EV_DEBUG << TAGDEL << PACKET(head) << " from output buffer\n";

        bool last = false;

        // check for multiflit packet
        if (not util::isTail(head, true)) {
            // keep single flit packet checking also in length mode
            cMessage *flit;
            for (int i = 1; i < head->getNum_flit(); ++i) {
                flit = (getOutputBuffer()->pop(&last));
                EV_DEBUG << TAGDEL << "flit[" << i << "] from output queue\n";
                delete flit;
            }

            // keep assertion also in length mode
            assert(util::isTail(flit, false));
        }
        EV_DEBUG << TAGDEL << "packet head\n";
        delete head;
        m_packet_in_transmission_id = -1; // reset id of currently active packet

        m_tx_flit = (getOutputBuffer()->pop(&last));
        assert((m_tx_flit == nullptr) || (util::isHead(m_tx_flit)));
        txCheckAndHandleHead();
    }
}

///// Handle outgoing packet's last flit.
// void AckNetworkInterface::txCheckAndHandleLast(bool last)
//{
//    if (txIsLast() || last) {
//        // The last flit of the current packet is out, schedule the
//        // corresponding timeout (if it is present).
//        scheduleTimeout(m_packet_in_transmission_id);
//        m_packet_in_transmission_id = -1; // reset id of currently active
//        packet
//    }
//}

// Should not happen
void NackNetworkInterface::handleIncomingAck(AckFlit *ack)
{
    // Acks should not get received in Nack based Network
    delete ack;
    assert(false);
}

// Retransmit if Nack received
void NackNetworkInterface::handleIncomingNack(NackFlit *nack)
{
    int dest_id = nack->getDestination();

    // check if nack is coming due to rerouting --> send again
    if (dest_id != m_id)
        handleNewAck(nack);

    else {
        long packet_id = nack->getPacket_id();

        EV_INFO << TAGRX << "nack<";
        EV_INFO << packet_id << ">\n";

        m_timeout = m_pending_buffer.getTimeout(packet_id);
        if (m_timeout != nullptr) {
            cancelEvent(m_timeout); // cancel event
        }

        EV_INFO << TAGTRIG << "nack ";
        EV_INFO << "for packet<" << packet_id << ">\n";

        NocPacket *packet = m_pending_buffer.getPacket(packet_id, true);
        if (packet != nullptr) {
            bool was_empty = getOutputBuffer()->duplicateAndPush(*packet);
            if (was_empty) {
                triggerTransmission(
                    m_t_clk); // trigger transmission immediately
            }
        } else {
            EV_DEBUG << TAGNOTEX << "packet<" << packet_id
                     << "> in pending buffer\n";
            assert(false && "NACK arrived after the delete timeout expired");
        }

        // record nack statistics
        simtime_t delay_generation =
            (nack->getArrivalTime() - nack->getGeneration_time());
        if ((int)(delay_generation / m_t_clk) > 0)
            emit(m_nack_latency_generation, (int)(delay_generation / m_t_clk));
        delete nack;
    }
}

void NackNetworkInterface::handleIncomingCNack(CNackFlit *cnack)
{
    int number_of_nacks = cnack->getNum_nacks_combined();

    EV_INFO << TAGRX << "cnack with ";
    EV_INFO << number_of_nacks << " nacks\n";

    // first extract nacks from cnack
    std::vector<NackFlit *> nack;
    nack.resize(number_of_nacks);
    for (int i = 0; i < number_of_nacks; i++) {
        nack[i] = cnack->popNack();
        // change ownership
        take(nack[i]);
    }
    assert(cnack->getNum_nacks_combined() ==
           0); // all nacks should be stored in nack[]

    delete cnack;
    cnack = nullptr;

    for (int i = 0; i < number_of_nacks; i++) {
        handleIncomingNack(nack[i]);
    }
}

void NackNetworkInterface::handleTimeout(TimeoutMsg *msg)
{
    EV_INFO << TAGRX << "Timeout<";
    EV_INFO << msg->getPacket_id() << ">\n";

    // delete pending packet (timeout and packet)
    m_pending_buffer.eraseAndDelete(msg->getPacket_id());
}

void NackNetworkInterface::txCheckAndHandleHeadChild(HeadFlit *head) {}

/// Handle outgoing head flit.
/// Schedule timeout
// void NackNetworkInterface::txCheckAndHandleHead()
//{
//    if ((m_tx_flit != nullptr) && (util::isHead(m_tx_flit))) {
//        HeadFlit *head = check_and_cast<HeadFlit *>(m_tx_flit);
//        m_packet_in_transmission_id = head->getPacket_id();
//        NocPacket *packet =
//            m_pending_buffer.getPacket(m_packet_in_transmission_id, false);
//
//        if (packet != nullptr) {
//            // set Timeout
//            scheduleTimeout(m_packet_in_transmission_id);
//            // if packet is injected the first time, set the injection time
//            // subsequent head flits will get the injection time from the
//            dup()
//            // of the pending buffer
//            if (head->getAttempts() < 1) {
//                packet->head()->setInjection_time(simTime());
//                head->setInjection_time(packet->head()->getInjection_time());
//            }
//        }
//    }
//}

// Flag based Network Interface
bool FlagNetworkInterface::isNewLast(cMessage *flit)
{
    return util::isTail(flit, true);
}

bool FlagNetworkInterface::txIsLast() { return util::isTail(m_tx_flit, true); }

BlockingNetworkInterface::~BlockingNetworkInterface()
{
    EV_DEBUG << "[~NI] size: " << getOutputBuffer()->packetQueueSize() << "/"
             << getOutputBuffer()->ackQueueSize() << "\n";
    getOutputBuffer()->clear();
}
OutputBufferBase *BlockingNetworkInterface::getOutputBuffer()
{
    return &m_output_buffer;
}

NonblockingFlagNetworkInterface::~NonblockingFlagNetworkInterface()
{
    EV_DEBUG << "[~NI] size: " << getOutputBuffer()->packetQueueSize() << "/"
             << getOutputBuffer()->ackQueueSize() << "\n";
    getOutputBuffer()->clear();
}
NonblockingLengthNetworkInterface::~NonblockingLengthNetworkInterface()
{
    EV_DEBUG << "[~NI] size: " << getOutputBuffer()->packetQueueSize() << "/"
             << getOutputBuffer()->ackQueueSize() << "\n";
    getOutputBuffer()->clear();
}
OutputBufferBase *NonblockingFlagNetworkInterface::getOutputBuffer()
{
    return &m_output_buffer;
}
OutputBufferBase *NonblockingLengthNetworkInterface::getOutputBuffer()
{
    return &m_output_buffer;
}

Define_Module(BlockingFlagAckNetworkInterface);
Define_Module(BlockingFlagNackNetworkInterface);
Define_Module(NonblockingFlagAckNetworkInterface);
Define_Module(NonblockingFlagNackNetworkInterface);
Define_Module(NonblockingLengthAckNetworkInterface);
Define_Module(NonblockingLengthNackNetworkInterface);
