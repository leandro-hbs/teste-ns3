/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2020 Universita' degli Studi di Napoli Federico II
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Stefano Avallone <stavallo@unina.it>
 */

#ifndef FRAME_EXCHANGE_MANAGER_H
#define FRAME_EXCHANGE_MANAGER_H

#include "ns3/object.h"
#include "qos-txop.h"
#include "wifi-tx-vector.h"
#include "wifi-psdu.h"
#include "regular-wifi-mac.h"
#include "mac-rx-middle.h"
#include "mac-tx-middle.h"
#include "wifi-phy.h"
#include "wifi-tx-timer.h"
#include "wifi-tx-parameters.h"

namespace ns3 {

class ChannelAccessManager;
class WifiProtectionManager;
struct WifiProtection;
class WifiAckManager;
struct WifiAcknowledgment;

/**
 * \ingroup wifi
 *
 * FrameExchangeManager is a base class handling the basic frame exchange
 * sequences for non-QoS stations.
 */
class FrameExchangeManager : public Object
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);
  FrameExchangeManager ();
  virtual ~FrameExchangeManager ();

  /**
   * Request the FrameExchangeManager to start a frame exchange sequence.
   *
   * \param dcf the channel access function that gained channel access. It is
   *            the DCF on non-QoS stations and an EDCA on QoS stations.
   * \return true if a frame exchange sequence was started, false otherwise
   */
  virtual bool StartTransmission (Ptr<Txop> dcf);

  /**
   * This method is intended to be called by the PHY layer every time an MPDU
   * is received and also when the reception of an A-MPDU is completed. In case
   * the PSDU contains multiple MPDUs, the <i>perMpduStatus</i> vector is empty
   * when receiving the individual MPDUs.
   *
   * \param psdu the received PSDU
   * \param rxSnr snr of MPDU received in linear scale
   * \param txVector TxVector of the received PSDU
   * \param perMpduStatus per MPDU reception status
   */
  void Receive (Ptr<WifiPsdu> psdu, double rxSnr,
                WifiTxVector txVector, std::vector<bool> perMpduStatus);

  /**
   * Set the MAC layer to use.
   *
   * \param mac the MAC layer to use
   */
  virtual void SetWifiMac (const Ptr<RegularWifiMac> mac);
  /**
   * Set the MAC TX Middle to use.
   *
   * \param txMiddle the MAC TX Middle to use
   */
  virtual void SetMacTxMiddle (const Ptr<MacTxMiddle> txMiddle);
  /**
   * Set the MAC RX Middle to use.
   *
   * \param rxMiddle the MAC RX Middle to use
   */
  virtual void SetMacRxMiddle (const Ptr<MacRxMiddle> rxMiddle);
  /**
   * Set the channel access manager to use
   *
   * \param channelAccessManager the channel access manager to use
   */
  virtual void SetChannelAccessManager (const Ptr<ChannelAccessManager> channelAccessManager);
  /**
   * Set the PHY layer to use.
   *
   * \param phy the PHY layer to use
   */
  virtual void SetWifiPhy (const Ptr<WifiPhy> phy);
  /**
   * Set the MAC address.
   *
   * \param address the MAC address
   */
  virtual void SetAddress (Mac48Address address);
  /**
   * Set the Basic Service Set Identification.
   *
   * \param bssid the BSSID
   */
  virtual void SetBssid (Mac48Address bssid);

  /**
   * Get the Protection Manager used by this node.
   *
   * \return the Protection Manager used by this node
   */
  Ptr<WifiProtectionManager> GetProtectionManager (void) const;

  /**
   * Calculate the time required to protect a frame according to the given
   * protection method. The protection time is stored in the protection
   * object itself.
   *
   * \param protection the protection method
   */
  virtual void CalculateProtectionTime (WifiProtection* protection) const;

  /**
   * Get the Acknowledgment Manager used by this node.
   *
   * \return the Acknowledgment Manager used by this node
   */
  Ptr<WifiAckManager> GetAckManager (void) const;

  /**
   * Calculate the time required to acknowledge a frame according to the given
   * acknowledgment method. The acknowledgment time is stored in the acknowledgment
   * object itself.
   *
   * \param acknowledgment the acknowledgment method
   */
  virtual void CalculateAcknowledgmentTime (WifiAcknowledgment* acknowledgment) const;

  /**
   * \param duration switching delay duration.
   *
   * This method is typically invoked by the PhyMacLowListener to notify
   * the MAC layer that a channel switching occurred. When a channel switching
   * occurs, pending MAC transmissions (RTS, CTS, Data and Ack) are cancelled.
   */
  void NotifySwitchingStartNow (Time duration);

  /**
   * This method is typically invoked by the PhyMacLowListener to notify
   * the MAC layer that the device has been put into sleep mode. When the device is put
   * into sleep mode, pending MAC transmissions (RTS, CTS, Data and Ack) are cancelled.
   */
  void NotifySleepNow (void);

  /**
   * This method is typically invoked by the PhyMacLowListener to notify
   * the MAC layer that the device has been put into off mode. When the device is put
   * into off mode, pending MAC transmissions (RTS, CTS, Data and Ack) are cancelled.
   */
  void NotifyOffNow (void);

protected:
  virtual void DoDispose ();

  /**
   * Fragment the given MPDU if needed. If fragmentation is needed, return the
   * first fragment; otherwise, return the given MPDU. Note that, if fragmentation
   * is applied, the given MPDU is dequeued from the MAC queue and the first
   * fragment is enqueued in its place.
   *
   * \param mpdu the given MPDU
   * \return the first fragment if fragmentation is needed, the given MPDU otherwise
   */
  Ptr<WifiMacQueueItem> GetFirstFragmentIfNeeded (Ptr<WifiMacQueueItem> mpdu);

  /**
   * Send an MPDU with the given TX parameters (with the specified protection).
   * Note that <i>txParams</i> is moved to m_txParams and hence is left in an
   * undefined state.
   *
   * \param mpdu the MPDU to send
   * \param txParams the TX parameters to use to transmit the MPDU
   */
  void SendMpduWithProtection (Ptr<WifiMacQueueItem> mpdu, WifiTxParameters& txParams);

  /**
   * Update the NAV, if needed, based on the Duration/ID of the given <i>psdu</i>.
   *
   * \param psdu the received PSDU
   * \param txVector TxVector of the received PSDU
   */
  virtual void UpdateNav (Ptr<const WifiPsdu> psdu, const WifiTxVector& txVector);

  /**
   * Reset the NAV upon expiration of the NAV reset timer.
   */
  virtual void NavResetTimeout (void);

  /**
   * This method handles the reception of an MPDU (possibly included in an A-MPDU)
   *
   * \param mpdu the received MPDU
   * \param rxSnr snr of MPDU received in linear scale
   * \param txVector TxVector of the received PSDU
   * \param inAmpdu true if the MPDU is part of an A-MPDU
   */
  virtual void ReceiveMpdu (Ptr<WifiMacQueueItem> mpdu, double rxSnr,
                            const WifiTxVector& txVector, bool inAmpdu);

  /**
   * This method is called when the reception of an A-MPDU including multiple
   * MPDUs is completed.
   *
   * \param psdu the received PSDU
   * \param rxSnr snr of MPDU received in linear scale
   * \param txVector TxVector of the received PSDU
   * \param perMpduStatus per MPDU reception status
   */
  virtual void EndReceiveAmpdu (Ptr<const WifiPsdu> psdu, double rxSnr,
                                const WifiTxVector& txVector, const std::vector<bool>& perMpduStatus);

  /**
   * Perform the actions needed when a Normal Ack is received.
   *
   * \param mpdu the MPDU that was acknowledged
   * \param txVector the TXVECTOR used to transmit the MPDU that was acknowledged
   * \param ackTxVector the TXVECTOR used to transmit the Normal Ack frame
   * \param rxSnr snr of MPDU received in linear scale
   * \param snr the SNR at the receiver for the MPDU that was acknowledged
   */
  virtual void ReceivedNormalAck (Ptr<WifiMacQueueItem> mpdu, const WifiTxVector& txVector,
                                  const WifiTxVector& ackTxVector, double rxSnr, double snr);

  /**
   * Notify other components that an MPDU was acknowledged.
   *
   * \param mpdu the MPDU that was acknowledged
   */
  virtual void NotifyReceivedNormalAck (Ptr<WifiMacQueueItem> mpdu);

  /**
   * Retransmit an MPDU that was not acknowledged.
   *
   * \param mpdu the MPDU to retransmit
   */
  virtual void RetransmitMpduAfterMissedAck (Ptr<WifiMacQueueItem> mpdu) const;

  /**
   * Retransmit an MPDU that was not sent because a CTS was not received.
   *
   * \param mpdu the MPDU to retransmit
   */
  virtual void RetransmitMpduAfterMissedCts (Ptr<WifiMacQueueItem> mpdu) const;

  /**
   * Pass the packet included in the given MPDU to the
   * packet dropped callback.
   *
   * \param mpdu the discarded MPDU
   */
  virtual void NotifyPacketDiscarded (Ptr<const WifiMacQueueItem> mpdu);

  /**
   * Perform actions that are possibly needed when receiving any frame,
   * independently of whether the frame is addressed to this station
   * (e.g., setting the NAV or the TXOP holder).
   *
   * \param psdu the received PSDU
   * \param txVector TX vector of the received PSDU
   */
  virtual void PreProcessFrame (Ptr<const WifiPsdu> psdu, const WifiTxVector& txVector);

  /**
   * Get the updated TX duration of the frame associated with the given TX
   * parameters if the size of the PSDU addressed to the given receiver
   * becomes <i>ppduPayloadSize</i>.
   *
   * \param ppduPayloadSize the new PSDU size
   * \param receiver the MAC address of the receiver of the PSDU
   * \param txParams the TX parameters
   * \return the updated TX duration
   */
  virtual Time GetTxDuration (uint32_t ppduPayloadSize, Mac48Address receiver,
                                 const WifiTxParameters& txParams) const;

  /**
   * Update the TX duration field of the given TX parameters after that the PSDU
   * addressed to the given receiver has changed.
   *
   * \param receiver the MAC address of the receiver of the PSDU
   * \param txParams the TX parameters
   */
  void UpdateTxDuration (Mac48Address receiver, WifiTxParameters& txParams) const;

  Ptr<Txop> m_dcf;                                  //!< the DCF/EDCAF that gained channel access
  WifiTxTimer m_txTimer;                            //!< the timer set upon frame transmission
  EventId m_navResetEvent;                          //!< the event to reset the NAV after an RTS
  Ptr<RegularWifiMac> m_mac;                        //!< the MAC layer on this station
  Ptr<MacTxMiddle> m_txMiddle;                      //!< the MAC TX Middle on this station
  Ptr<MacRxMiddle> m_rxMiddle;                      //!< the MAC RX Middle on this station
  Ptr<ChannelAccessManager> m_channelAccessManager; //!< the channel access manager
  Ptr<WifiPhy> m_phy;                               //!< the PHY layer on this station
  Mac48Address m_self;                              //!< the MAC address of this device
  Mac48Address m_bssid;                             //!< BSSID address (Mac48Address)
  Time m_navEnd;                                    //!< NAV expiration time

  /**
   * Forward an MPDU down to the PHY layer.
   *
   * \param mpdu the MPDU to forward down
   * \param txVector the TXVECTOR used to transmit the MPDU
   */
  virtual void ForwardMpduDown (Ptr<WifiMacQueueItem> mpdu, WifiTxVector& txVector);

  /**
   * Dequeue the given MPDU from the queue in which it is stored.
   *
   * \param mpdu the given MPDU
   */
  virtual void DequeueMpdu (Ptr<WifiMacQueueItem> mpdu);

  /**
   * Compute how to set the Duration/ID field of a frame being transmitted with
   * the given TX parameters
   *
   * \param header the MAC header of the frame
   * \param size the size of the frame in bytes
   * \param txParams the TX parameters used to send the frame
   * \param fragmentedPacket the packet that originated the frame to transmit, in case
   *                         the latter is a fragment
   * \return the computed Duration/ID value
   */
  virtual Time GetFrameDurationId (const WifiMacHeader& header, uint32_t size,
                                   const WifiTxParameters& txParams,
                                   Ptr<Packet> fragmentedPacket) const;

  /**
   * Compute how to set the Duration/ID field of an RTS frame to send to protect
   * a frame transmitted with the given TX vector.
   *
   * \param rtsTxVector the TX vector used to send the RTS frame
   * \param txDuration the TX duration of the data frame
   * \param response the time taken by the response (acknowledgment) to the data frame
   * \return the computed Duration/ID value for the RTS frame
   */
  virtual Time GetRtsDurationId (const WifiTxVector& rtsTxVector, Time txDuration, Time response) const;

  /**
   * Send RTS to begin RTS-CTS-Data-Ack transaction.
   *
   * \param txParams the TX parameters for the data frame
   */
  void SendRts (const WifiTxParameters& txParams);

  /**
   * Send CTS after receiving RTS.
   *
   * \param rtsHdr the header of the received RTS
   * \param rtsTxMode the TX mode used to transmit the RTS
   * \param rtsSnr the SNR of the RTS in linear scale
   */
  void SendCtsAfterRts (const WifiMacHeader& rtsHdr, WifiMode rtsTxMode, double rtsSnr);

  /**
   * Send CTS after receiving RTS.
   *
   * \param rtsHdr the header of the received RTS
   * \param ctsTxVector the TXVECTOR to use to transmit the CTS
   * \param rtsSnr the SNR of the RTS in linear scale
   */
  void DoSendCtsAfterRts (const WifiMacHeader& rtsHdr, WifiTxVector& ctsTxVector, double rtsSnr);

  /**
   * Compute how to set the Duration/ID field of a CTS-to-self frame to send to
   * protect a frame transmitted with the given TX vector.
   *
   * \param ctsTxVector the TX vector used to send the CTS-to-self frame
   * \param txDuration the TX duration of the data frame
   * \param response the time taken by the response (acknowledgment) to the data frame
   * \return the computed Duration/ID value for the CTS-to-self frame
   */
  virtual Time GetCtsToSelfDurationId (const WifiTxVector& ctsTxVector, Time txDuration,
                                       Time response) const;

  /**
   * Send CTS for a CTS-to-self mechanism.
   *
   * \param txParams the TX parameters for the data frame
   */
  void SendCtsToSelf (const WifiTxParameters& txParams);

  /**
   * Send Normal Ack.
   *
   * \param hdr the header of the frame soliciting the Normal Ack
   * \param dataTxVector the TXVECTOR used to transmit the frame soliciting the Normal Ack
   * \param dataSnr the SNR of the frame soliciting the Normal Ack in linear scale
   */
  void SendNormalAck (const WifiMacHeader& hdr, const WifiTxVector& dataTxVector, double dataSnr);

  /**
   * Get the next fragment of the current MSDU.
   * Only called for fragmented MSDUs.
   *
   * \return the next fragment of the current MSDU.
   */
  Ptr<WifiMacQueueItem> GetNextFragment (void);

  /**
   * Take necessary actions upon a transmission success. A non-QoS station
   * transmits the next fragment, if any, or releases the channel, otherwise.
   */
  virtual void TransmissionSucceeded (void);

  /**
   * Take necessary actions upon a transmission failure. A non-QoS station
   * releases the channel when this method is called.
   */
  virtual void TransmissionFailed (void);

  /**
   * Called when the Ack timeout expires.
   *
   * \param mpdu the MPDU that solicited a Normal Ack response
   */
  void NormalAckTimeout (Ptr<WifiMacQueueItem> mpdu);

  /**
   * Called when the CTS timeout expires.
   */
  virtual void CtsTimeout (void);

private:
  /**
   * Create a Protection Manager of the given TypeId
   *
   * \param protectionManagerTypeId the TypeId of the Protection Manager to create
   */
  void CreateProtectionManager (TypeId protectionManagerTypeId);

  /**
   * Create an Acknowledgment Manager of the given TypeId
   *
   * \param acknowledgmentManagerTypeId the TypeId of the Acknowledgment Manager to create
   */
  void CreateAcknowledgmentManager (TypeId acknowledgmentManagerTypeId);

  /**
   * \param txVector the TXVECTOR decoded from PHY header.
   * \param psduDuration the duration of the PSDU that is about to be received.
   *
   * This method is typically invoked by the lower PHY layer to notify
   * the MAC layer that the reception of a PSDU is starting.
   * This is equivalent to the PHY-RXSTART primitive.
   * If the reception is correct for at least one MPDU of the PSDU
   * the Receive method will be called after \p psduDuration.
   */
  void RxStartIndication (WifiTxVector txVector, Time psduDuration);

  /**
   * Send the current MPDU, which can be acknowledged by a Normal Ack.
   */
  void SendMpdu (void);

  /**
   * Reset this frame exchange manager.
   */
  virtual void Reset (void);

  Ptr<WifiMacQueueItem> m_mpdu;                   //!< the MPDU being transmitted
  WifiTxParameters m_txParams;                    //!< the TX parameters for the current frame
  Ptr<Packet> m_fragmentedPacket;                 //!< the MSDU being fragmented
  bool m_moreFragments;                           //!< true if a fragment has to be sent after a SIFS
  TypeId m_protectionManagerTypeId;               //!< TypeId of the protection manager to install
  Ptr<WifiProtectionManager> m_protectionManager; //!< Protection manager
  TypeId m_acknowledgmentManagerTypeId;           //!< TypeId of the acknowledgment manager to install
  Ptr<WifiAckManager> m_ackManager;               //!< Acknowledgment manager
};

} //namespace ns3

#endif /* FRAME_EXCHANGE_MANAGER_H */