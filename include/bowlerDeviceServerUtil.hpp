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

#define BOWLER_ERROR INT32_MAX
#define DEFAULT_PACKET_SIZE 64
#define HEADER_LENGTH 3
#define DEFAULT_PAYLOAD_SIZE (DEFAULT_PACKET_SIZE - HEADER_LENGTH)

#if defined(PLATFORM_ESP32)
#define time_t int64_t
#elif defined(PLATFORM_TEENSY)
#define time_t uint32_t
#endif

time_t getTime();
