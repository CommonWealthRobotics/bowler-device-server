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

#include "bowlerPacket.hpp"
#include <Arduino.h>

namespace bowlerserver {
/**
 * A Packet which does nothing.
 */
class NoopPacket : public Packet {
  public:
  NoopPacket(std::uint8_t iid, bool iisReliable = false) : Packet(iid, iisReliable) {
  }

  std::int32_t event(std::uint8_t *payload) override {
    return 1;
  }
};
} // namespace bowlerserver
