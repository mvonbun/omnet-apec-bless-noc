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
// File      : network_interface.h
// Created   : 11 Apr 2017 for apec_bless_noc
// Author(s) : Michael Vonbun
// E-mail(s) : michael.vonbun@tum.de
// 

#ifndef __APEC_BLESS_NOC_NETWORK_INTERFACE_H_
#define __APEC_BLESS_NOC_NETWORK_INTERFACE_H_

// stl
#include <map>           // std::map
#include <unordered_map> // std::unordered_map
#include <unordered_set> // std::unordered_set
#include <utility>       // std::pair, std::get<> pair
#include <vector>        // std::vector

// omnet
#include <omnetpp.h>

// project utilities
#include <debug_messages.h>     // TAGs, PACKET and FLIT macro
#include <enum_definitions_m.h> // scheduling priorities, message types

// project messages
class BaseFlit;
class HeadFlit;
class AckFlit;
class TimeoutMsg;
class NocPacket;

// module
#include <output_buffer.h>
#include <pending_buffer.h>

using namespace omnetpp;

/** Network Interface.
 *
 * SOURCE GATE
 *  Behavior:
 *      Receives packets from source and enqueues them
 *      in the packet buffer. Also prepares retransmission
 *      scheduling.
 *  Implementation:
 *      Flits of a packet are stored in a temporary vector and
 *      pushed to an unordered map until the ACK for this packet
 *      arrived.
 *
 * PORT GATE
 * -> Receives packets
 *    Forwards them to the sink/decoder
 *
 *
 * Generates ACK and enqueues them in the ACK OUT queue.
 * Forwards HEAD to core to calculate statistics.
 *
 * Sends FLITS by multiplexing the two queues ACK OUT and PACKET OUT.
 * When sending a PACKET, sends the head to one self after timeout.
 *
 */
class NetworkInterface : public cSimpleModule
{
public:
    NetworkInterface();
    virtual ~NetworkInterface();

protected:
    /** @name Omnet Interface. */
    //@{
    virtual void initialize() override;
    virtual void finish() override;
    virtual void handleMessage(cMessage *msg) override;
    //@}

protected:
    /** @name Module buffers. */
    //@{
    NocPacket *m_new_packet;        ///< New packet buffer.
    PendingBuffer m_pending_buffer; ///< Retransmission buffer.
    //@}

    /** @name Pure virtual output buffer. */
    //@{
    virtual OutputBufferBase *getOutputBuffer() = 0;
    //@}

    /** @name Module parameters. */
    //@{
    int m_id;                         ///< Module id.
    simtime_t m_t_clk;                ///< System clock.
    size_t m_max_num_pending_packets; ///< Maximum number of pending packets.
    /// Late ACKs may arrive.
    /// If compensation is enabled,
    /// an already enqueued packet is deleted from the output buffer.
    bool m_late_ack_compensation_enabled;
    /// terminate simulation when pending buffer is full
    bool m_end_simulation_on_full_buffer;
    //@}

    /** @name Module states. */
    //@{
    /// Id of packet currently being transmitted.
    long m_packet_in_transmission_id;
    //@}

    /** @name Messages and events to reuse. */
    //@{
    cMessage *m_transmit_event; ///< Transmission event.
    TimeoutMsg *m_timeout;      ///< Used to fetch and process timeouts.
    cMessage *m_tx_flit;        ///< Flit to be transmitted.
    //@}

    /** @name Buffer signals / statistics. */
    //@{
    /// Retransmission buffer.
    simsignal_t m_pending_buffer_num_packets;
    simsignal_t m_pending_buffer_num_flits;
    /// Output queues.
    simsignal_t m_output_queue_num_packet_flits;
    simsignal_t m_output_queue_num_acks;
    //@}

    /** @name Incoming ACK based signals / statistics. */
    //@{
    /// ACKed packets.
    simsignal_t m_ack_packet_attempts;
    simsignal_t m_ack_packet_latency_generation;
    simsignal_t m_ack_packet_latency_injection;
    /// ACKs
    simsignal_t m_num_late_acks;
    simsignal_t m_ack_latency_generation;
    simsignal_t m_ack_latency_injection;
    simsignal_t m_ack_latency_network_access;
    //@}

    /// NACKs
    simsignal_t m_nack_latency_generation;
    //@}

    /** @name Ni flit rates. */
    /// Real injection rate, measured at the network port.
    simsignal_t m_num_flit_injected;
    simsignal_t m_num_ack_injected;
    simsignal_t m_num_packet_injected;
    simsignal_t m_num_tail_injected;
    /// Real generation rate,
    /// i.e. generated packets that are actually enqueued in the pending buffer.
    /// Measured at enc->ni port / pending buffer interface.
    simsignal_t m_num_flit_generation_rate_payload;
    simsignal_t m_num_flit_generation_rate;
    //@}

protected:
    /** @name Incoming message handling. */
    //@{
    void handleNewPacket(cMessage *msg); ///< From source port (new packet).
    void handleNewAck(cMessage *msg);    ///< From sink port (outgoing acks).
    void handleIncomingFlits(cMessage *msg); ///< From network port (incoming).
    //@}

    /** @name Handle new packets subroutines. */
    //@{
    void handleNewHead(HeadFlit *head);
    void handleNewFlit(cMessage *flit);
    virtual bool isNewLast(cMessage *flit) = 0;
    void handleNewLastflit();
    //@}

    /** @name Handle network flits subroutines. */
    //@{
    virtual void handleIncomingAck(AckFlit *ack) = 0;
    virtual void handleIncomingNack(NackFlit *nack) = 0;
    virtual void handleIncomingCNack(CNackFlit *cnack) = 0;
    void handleIncomingOther(cMessage *msg);
    //@}

    /** @name Handle timeouts. */
    //@{
    void scheduleTimeout(const long id);
    virtual void handleTimeout(TimeoutMsg *msg) = 0;
    //@}

    /** @name Flit injection. */
    //@{
    void popAndSend(); ///< Main transmission function.
    void triggerTransmission(const simtime_t time); ///< Transmission trigger.
    virtual void
    txCheckAndHandleHead(); ///< Packet transmission start handling.
    virtual void
    txCheckAndHandleHeadChild(HeadFlit *head) = 0; ///< Child adaptation.
    virtual void
    txCheckAndHandleLast(bool last); ///< Packet transmission stop handling.
    void txCheckAndHandleAck();      ///< Ack transmission handling.
    void txCheckAndHandleNack();     ///< Nack transmission handling.
    virtual bool txIsLast() = 0;
    //@}
};

/** ACK Network Interface.
 *
 *  Pure virtual.
 *  Uses ACKs to delete packets in pending buffer and timeouts to trigger
 *  retransmissions.
 *
 *  Timeouts are scheduled
 */
class AckNetworkInterface : public virtual NetworkInterface
{
protected:
    virtual void handleIncomingAck(AckFlit *ack) override;
    virtual void handleIncomingNack(NackFlit *nack) override;
    virtual void handleIncomingCNack(CNackFlit *cnack) override;
    virtual void handleTimeout(TimeoutMsg *msg) override;
    virtual void txCheckAndHandleHeadChild(HeadFlit *head) override;
};

/** NACK Network Interface.
 *
 *  Pure virtual.
 *  Uses NACKs to trigger retransmissions and timeouts to delete packets in
 *  pending buffer.
 *
 *  Timeouts are scheduled with head flits at the output FIFO.
 */
class NackNetworkInterface : public virtual NetworkInterface
{
protected:
    virtual void handleIncomingAck(AckFlit *ack) override;
    virtual void handleIncomingNack(NackFlit *nack) override;
    virtual void handleIncomingCNack(CNackFlit *cnack) override;
    virtual void handleTimeout(TimeoutMsg *msg) override;
    virtual void txCheckAndHandleHeadChild(HeadFlit *head) override;
};

/** Flag Network Interface.
 *
 *  Pure virtual.
 *  Uses packets with a tail-flag mechanism.
 */
class FlagNetworkInterface : public virtual NetworkInterface
{
protected:
    virtual bool isNewLast(cMessage *flit) override;
    virtual bool txIsLast() override;
};

/** Blocking Network Interface.
 *
 *  Pure virtual.
 *  Only entire packets are leaving the network interface, i.e. the output port
 *  is blocked until an entire packet has been sent.
 */
class BlockingNetworkInterface : public virtual NetworkInterface
{
public:
    BlockingNetworkInterface(){};
    virtual ~BlockingNetworkInterface();

protected:
    virtual OutputBufferBase *getOutputBuffer() override;

private:
    OutputBufferBlocking m_output_buffer;
};

/** Nonblocking network interface.
 *
 *  Pure virtual.
 *  ACKs/NACKs are sent as soon as possible and can interfere with an ongoing
 *  packet transmission.
 *  I think right now, this is only feasible for tail mechanism NoCs.
 */
class NonblockingNetworkInterface : public virtual NetworkInterface
{
};

/** Blocking Flag Network Interface.
 *
 *  Also used for Blocking Length Network Interface, as there are only changes
 *  in the routers.
 */
class BlockingFlagNetworkInterface : public virtual BlockingNetworkInterface,
                                     public virtual FlagNetworkInterface
{
};

/** Nonblocking Flag Network Interface.
 *
 *  Only works in preserving mode.
 */
class NonblockingFlagNetworkInterface
    : public virtual NonblockingNetworkInterface,
      public virtual FlagNetworkInterface
{
public:
    NonblockingFlagNetworkInterface(){};
    virtual ~NonblockingFlagNetworkInterface();

protected:
    virtual OutputBufferBase *getOutputBuffer() override;

private:
    OutputBufferNonblockingPreserving m_output_buffer;
};

/** Nonblocking Flag Network Interface.
 *
 *  Only works in nonpreserving mode.
 */
class NonblockingLengthNetworkInterface
    : public virtual NonblockingNetworkInterface,
      public virtual FlagNetworkInterface
{
public:
    NonblockingLengthNetworkInterface(){};
    virtual ~NonblockingLengthNetworkInterface();

protected:
    virtual OutputBufferBase *getOutputBuffer() override;

private:
    OutputBufferNonblockingNonpreserving m_output_buffer;
};

/** Blocking Flag ACK Network Interface.
 *
 *  Blocking is the same for tail-flag and packet-length modes, so both types
 *  uses this class.
 */
class BlockingFlagAckNetworkInterface : public BlockingFlagNetworkInterface,
                                        public AckNetworkInterface
{
};

/** Blocking Flag NACK Network Interface.
 *
 *  Blocking is the same for tail-flag and packet-length modes, so both types
 *  uses this class.
 */
class BlockingFlagNackNetworkInterface : public BlockingFlagNetworkInterface,
                                         public NackNetworkInterface
{
};

/** Nonblocking Flag ACK Network Interface.
 *  Like NonblockingFlag NI which uses ACKs.
 */
class NonblockingFlagAckNetworkInterface
    : public NonblockingFlagNetworkInterface,
      public AckNetworkInterface
{
};

/** Nonblocking Flag NACK Network Interface.
 *  Like NonblockingFlag NI which uses NACKs.
 */
class NonblockingFlagNackNetworkInterface
    : public NonblockingFlagNetworkInterface,
      public NackNetworkInterface
{
};

/** Nonblocking Length ACK Network Interface.
 *  Like NonblockingLength NI which uses ACKs.
 */
class NonblockingLengthAckNetworkInterface
    : public NonblockingLengthNetworkInterface,
      public AckNetworkInterface
{
};

/** Nonblocking Length NACK Network Interface.
 *  Like NonblockingLength NI which uses NACKs.
 */
class NonblockingLengthNackNetworkInterface
    : public NonblockingLengthNetworkInterface,
      public NackNetworkInterface
{
};

#endif
