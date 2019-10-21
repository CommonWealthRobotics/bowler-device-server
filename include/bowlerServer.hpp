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

#include <array>
#include <cstdint>

/**
 * The server used to interact with the PC side.
 */
template <std::size_t N> class BowlerServer {
  public:
  virtual ~BowlerServer() = default;

  /**
   * Writes data to the PC.
   *
   * @param ipayload The payload to write data from.
   * @return `1` on success or BOWLER_ERROR on error.
   */
  virtual std::int32_t write(std::array<std::uint8_t, N> ipayload) = 0;

  /**
   * Reads data from the PC.
   *
   * @param ipayload The payload to write the data into.
   * @return `1` on success or BOWLER_ERROR on error.
   */
  virtual std::int32_t read(std::array<std::uint8_t, N> &ipayload) = 0;

  /**
   * Checks if there is data available to read.
   *
   * @param iavailable The bool to write the result to.
   * @return `1` on success or BOWLER_ERROR on error.
   */
  virtual std::int32_t isDataAvailable(bool &iavailable) = 0;
};
