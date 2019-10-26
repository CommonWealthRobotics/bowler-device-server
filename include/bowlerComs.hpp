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
#include <array>
#include <functional>
#include <memory>
#include <vector>

namespace bowlerserver {
template <std::size_t N> class BowlerComs {
  public:
  virtual ~BowlerComs() = default;

  virtual void addEnsuredPacket(std::function<std::shared_ptr<Packet>(void)> iaddPacket) = 0;

  virtual std::int32_t addEnsuredPackets() = 0;

  /**
   * Adds a packet event handler. The packet id cannot already be used.
   *
   * @param ipacket The packet event handler.
   * @return `1` on success or BOWLER_ERROR on error.
   */
  virtual std::int32_t addPacket(std::shared_ptr<Packet> ipacket) = 0;

  /**
   * Removes a packet event handler.
   *
   * @param iid The id of the packet.
   */
  virtual void removePacket(const std::uint8_t iid) = 0;

  /**
   * @return Every attached packet id.
   */
  virtual std::vector<std::uint8_t> getAllPacketIDs() = 0;

  /**
   * Run an iteration of coms.
   *
   * @return `1` on success or BOWLER_ERROR on error.
   */
  virtual std::int32_t loop() = 0;
};
} // namespace bowlerserver
