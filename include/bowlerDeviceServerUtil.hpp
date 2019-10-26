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

#include "errno.h"
#include <Arduino.h>

namespace bowler {
const std::int32_t BOWLER_ERROR = INT32_MAX;

const std::int32_t DEFAULT_PACKET_SIZE = 64;
const std::int32_t HEADER_LENGTH = 3;
const std::int32_t DEFAULT_PAYLOAD_SIZE = DEFAULT_PACKET_SIZE - HEADER_LENGTH;

const std::uint8_t SERVER_MANAGEMENT_PACKET_ID = 1;

const std::uint8_t OPERATION_DISCONNECT_ID = 1;

const std::uint8_t STATUS_ACCEPTED = 1;
const std::uint8_t STATUS_REJECTED_GENERIC = 2;

#if defined(PLATFORM_ESP32)
using time_t = int64_t;
#elif defined(PLATFORM_TEENSY)
using time_t = uint32_t;
#endif

time_t getTime();
} // namespace bowler
