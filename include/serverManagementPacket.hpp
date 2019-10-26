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
#include <Arduino.h>

namespace bowler {
/**
 * A Packet which performs server management operations.
 */
template <std::size_t N> class ServerManagementPacket : public Packet {
  public:
  ServerManagementPacket(BowlerComs<N> *icoms)
    : Packet(SERVER_MANAGEMENT_PACKET_ID, true), coms(icoms) {
  }

  std::int32_t event(std::uint8_t *payload) override {
    const std::uint8_t operation = payload[0];

    switch (operation) {
    case OPERATION_DISCONNECT_ID: {
      for (auto &&id : coms->getAllPacketIDs()) {
        coms->removePacket(id);
      }
    }

    default: {
      errno = EINVAL;
      return BOWLER_ERROR;
    }
    }

    return 1;
  }

  private:
  BowlerComs<N> *coms;
};
} // namespace bowler
