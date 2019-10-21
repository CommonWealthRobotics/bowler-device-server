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

#include "bowlerServer.hpp"
#include <queue>

template <std::size_t N> class MockBowlerServer : public BowlerServer<N> {
  public:
  std::int32_t write(std::array<std::uint8_t, N> payload) override {
    writesReceived.push(payload);
    return 1;
  }

  std::int32_t read(std::array<std::uint8_t, N> &payload) override {
    payload = readsToSend.front();
    readsToSend.pop();
    return 1;
  }

  std::int32_t isDataAvailable(bool &available) override {
    available = readsToSend.size() > 0;
    return 1;
  }

  std::queue<std::array<std::uint8_t, N>> writesReceived;
  std::queue<std::array<std::uint8_t, N>> readsToSend;
};
