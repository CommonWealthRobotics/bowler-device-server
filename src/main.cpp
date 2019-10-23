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
#if !defined(UNIT_TEST)

#include "bowlerServerController.hpp"
#include <Arduino.h>
#include <Esp32WifiManager.h>

BowlerServerController<DEFAULT_PACKET_SIZE> *controller;

void setup() {
  controller = new BowlerServerController<DEFAULT_PACKET_SIZE>();
  controller->getComs().addPacket(
    std::unique_ptr<NoopPacket<DEFAULT_PACKET_SIZE>>(new NoopPacket<DEFAULT_PACKET_SIZE>(2, true)));
}

void loop() {
  controller->loop();
}

#endif
