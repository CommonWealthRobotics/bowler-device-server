/*
 * This file is part of bowler-device-server.
 *
 * bowler-device-server is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * bowler-device-server is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with bowler-device-server.  If not, see <https://www.gnu.org/licenses/>.
 */
#pragma once

#include "bowlerDeviceServerUtil.hpp"
#include "bowlerPacket.hpp"
#include "bowlerServer.hpp"
#include <array>
#include <map>
#include <memory>

#define SERVER_MANAGEMENT_PACKET_ID 1

/**
 * Buffer format is:
 * <ID (1 byte)> <Seq Num (1 byte)> <ACK num (1 byte)> <Payload (N bytes)>.
 */
template <std::size_t N> class BowlerComs {
  // The entire packet length must be at least the header length plus one payload byte
  static_assert(N >= HEADER_LENGTH + 1,
                "Packet length must be at least the header length plus one payload byte.");

  public:
  BowlerComs(std::unique_ptr<BowlerServer<N>> iserver) : server(std::move(iserver)) {
  }

  virtual ~BowlerComs() = default;

  /**
   * Adds a packet event handler. The packet id cannot be equal to the
   * SERVER_MANAGEMENT_PACKET_ID. The packet id cannot already be used.
   *
   * @param ipacket The packet event handler.
   * @return `1` on success or BOWLER_ERROR on error.
   */
  std::int32_t addPacket(const std::shared_ptr<Packet> &ipacket) {
    if (ipacket->getId() == SERVER_MANAGEMENT_PACKET_ID) {
      // Can't overlap with the management packet id
      errno = EINVAL;
      return BOWLER_ERROR;
    }

    if (packets.find(ipacket->getId()) == packets.end()) {
      packets[ipacket->getId()] = ipacket;
    } else {
      // The packet id is already used
      errno = EINVAL;
      return BOWLER_ERROR;
    }

    return 1;
  }

  /**
   * Removes a packet event handler.
   *
   * @param iid The id of the packet.
   */
  void removePacket(const std::uint8_t iid) {
    packets.erase(iid);
  }

  /**
   * Run an iteration of coms.
   *
   * @return `1` on success or BOWLER_ERROR on error.
   */
  std::int32_t loop() {
    bool isDataAvailable;
    std::int32_t error = server->isDataAvailable(isDataAvailable);
    if (error != BOWLER_ERROR) {
      if (isDataAvailable) {
        std::array<std::uint8_t, N> data;

        std::int32_t error = server->read(data);
        if (error != BOWLER_ERROR) {
          auto id = getPacketId(data);
          auto packet = packets.find(id);
          if (packet == packets.end()) {
            // The corresponding packet was not found, meaning there is no handler registered for
            // it. Clear the payload and reply.
            std::fill(std::next(data.begin(), HEADER_LENGTH), data.end(), 0);

            auto writeError = server->write(data);
            if (writeError == BOWLER_ERROR) {
              Serial.printf(
                "Error while replying to unregistered packet: %d %s\n", errno, strerror(errno));
            }

            errno = ENODEV;
            return BOWLER_ERROR;
          } else {
            // The packet handler was found
            if (packet->second->isReliable()) {
              handlePacketReliable(packet, data);
            } else {
              handlePacketUnreliable(packet, data);
            }
          }
        } else {
          // Error reading data
          Serial.printf("Error reading: %d %s\n", errno, strerror(errno));
        }
      }
    } else {
      // Error running isDataAvailable. EWOULDBLOCK is typical of having no data (not really an
      // error).
      if (errno != EWOULDBLOCK) {
        Serial.printf("Error peeking: %d %s\n", errno, strerror(errno));
      }
    }

    return 1;
  }

  protected:
  /**
   * Handles a packet for unreliable transport.
   *
   * @param idata Data that was just read from the receive buffer.
   */
  template <typename T>
  void handlePacketUnreliable(T &ipacket, std::array<std::uint8_t, N> &idata) {
    auto error = ipacket->second->event(idata.data() + HEADER_LENGTH);
    if (error == BOWLER_ERROR) {
      Serial.printf("Error handling packet event: %d %s\n", errno, strerror(errno));
    }

    error = server->write(idata);
    if (error == BOWLER_ERROR) {
      Serial.printf("Error writing: %d %s\n", errno, strerror(errno));
    }
  }

  /**
   * Handles a packet for reliable transport.
   *
   * @param idata Data that was just read from the receive buffer.
   */
  template <typename T> void handlePacketReliable(T &ipacket, std::array<std::uint8_t, N> &idata) {
    switch (state) {
    case waitForZero: {
      if (getSeqNum(idata) == 0) {
        // Right payload. Handle it.
        auto error = ipacket->second->event(idata.data() + HEADER_LENGTH);
        if (error == BOWLER_ERROR) {
          Serial.printf("Error handling packet event: %d %s\n", errno, strerror(errno));
        }

        // ACK it and start waiting for the next packet.
        setAckNum(idata, 0);
        error = server->write(idata);
        if (error == BOWLER_ERROR) {
          Serial.printf("Error writing: %d %s\n", errno, strerror(errno));
        }
        state = waitForOne;
      } else {
        // Wrong packet. Clear the payload and ACK 1.
        std::fill(std::next(idata.begin(), HEADER_LENGTH), idata.end(), 0);
        setAckNum(idata, 1);
        auto error = server->write(idata);
        if (error == BOWLER_ERROR) {
          Serial.printf("Error writing: %d %s\n", errno, strerror(errno));
        }
      }
      break;
    }

    case waitForOne: {
      if (getSeqNum(idata) == 1) {
        // Right payload. Handle it.
        auto error = ipacket->second->event(idata.data() + HEADER_LENGTH);
        if (error == BOWLER_ERROR) {
          Serial.printf("Error handling packet event: %d %s\n", errno, strerror(errno));
        }

        // ACK it and start waiting for the next packet.
        setAckNum(idata, 1);
        error = server->write(idata);
        if (error == BOWLER_ERROR) {
          Serial.printf("Error writing: %d %s\n", errno, strerror(errno));
        }
        state = waitForZero;
      } else {
        // Wrong packet. Clear the payload and ACK 0.
        std::fill(std::next(idata.begin(), HEADER_LENGTH), idata.end(), 0);
        setAckNum(idata, 0);
        auto error = server->write(idata);
        if (error == BOWLER_ERROR) {
          Serial.printf("Error writing: %d %s\n", errno, strerror(errno));
        }
      }
      break;
    }
    }
  }

  std::uint8_t getPacketId(const std::array<std::uint8_t, N> &idata) const {
    return idata.at(0);
  }

  std::uint8_t getSeqNum(const std::array<std::uint8_t, N> &idata) const {
    return idata.at(1);
  }

  std::uint8_t getAckNum(const std::array<std::uint8_t, N> &idata) const {
    return idata.at(2);
  }

  void setSeqNum(std::array<std::uint8_t, N> &idata, std::uint8_t iseqNum) const {
    idata.at(1) = iseqNum;
  }

  void setAckNum(std::array<std::uint8_t, N> &idata, std::uint8_t iackNum) const {
    idata.at(2) = iackNum;
  }

  enum states_t { waitForZero, waitForOne };
  states_t state{waitForZero};
  std::unique_ptr<BowlerServer<N>> server;
  std::map<std::uint8_t, std::shared_ptr<Packet>> packets;
};
