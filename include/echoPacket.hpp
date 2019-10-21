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

#include "packet.hpp"
#include <Arduino.h>

template <std::size_t N> class EchoPacket : public Packet<N> {
  public:
  EchoPacket(std::uint8_t iid) : Packet<N>(iid) {
  }

  std::int32_t event(std::array<std::uint8_t, N> payload) override {
    for (auto &&elem : payload) {
      Serial.printf("%u, ", elem);
    }
    Serial.print("\n");
    return 1;
  }
};
