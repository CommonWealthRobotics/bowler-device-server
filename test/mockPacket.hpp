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
#include <array>
#include <cstring>
#include <vector>

namespace bowlerserver {
/**
 * A Packet which records the payloads it receives.
 */
class MockPacket : public Packet {
  public:
  MockPacket(std::uint8_t iid, bool iisReliable = false) : Packet(iid, iisReliable) {
  }

  std::int32_t event(std::uint8_t *payload) override {
    std::array<std::uint8_t, DEFAULT_PAYLOAD_SIZE> copy;
    std::memcpy(copy.data(), payload, DEFAULT_PAYLOAD_SIZE * sizeof(payload[0]));
    payloads.push_back(copy);
    return 1;
  }

  std::vector<std::array<std::uint8_t, DEFAULT_PAYLOAD_SIZE>> payloads;
};
} // namespace bowlerserver
