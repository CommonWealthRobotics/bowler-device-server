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

#include "bowlerComs.hpp"
#include "noopPacket.hpp"
#include "udpServer.hpp"
#include "util.hpp"
#include <Arduino.h>
#include <Esp32WifiManager.h>

class BowlerServerController {
  public:
  void loop() {
    time_t time = getTime();

    // Ensure 0.5 ms spacing *between* reads for Wifi to transact. Also check for the wrapover case
    if (time - lastLoopTime > 500 || time < lastLoopTime) {
      switch (state) {
      case startup: {
        setup();
        state = waitForConnection;
        break;
      }

      case waitForConnection: {
#if defined(USE_WIFI)
        if (manager.getState() == Connected) {
          state = run;
        }
#elif defined(USE_HID)
        state = run;
#endif
        break;
      }

      default:
        break;
      }

      lastLoopTime = getTime();
    }

    if (state != startup) {
// If this is run before the sensor reads, the I2C will fail because the time it takes to send
// the UDP causes a timeout
#if defined(USE_WIFI)
      manager.loop();
      if (manager.getState() == Connected) {
        coms.loop();
      }
#elif defined(USE_HID)
#endif
    }
  }

  protected:
  void setup() {
    if (state != startup) {
      return;
    }

    state = waitForConnection;

#if defined(USE_WIFI)
    manager.setupAP();

    coms.addPacket(
      std::unique_ptr<NoopPacket<DEFAULT_PACKET_SIZE>>(new NoopPacket<DEFAULT_PACKET_SIZE>(1)));
#elif defined(USE_HID)
#else
    Serial.begin(115200);
#endif
  }

  private:
  enum state_t { startup, waitForConnection, run };

  state_t state;
  time_t lastLoopTime{0};

#if defined(USE_WIFI)
  WifiManager manager;
  BowlerComs<DEFAULT_PACKET_SIZE> coms{
    std::unique_ptr<UDPServer<DEFAULT_PACKET_SIZE>>(new UDPServer<DEFAULT_PACKET_SIZE>())};
#elif defined(USE_HID)
#error "BowlerServerController not implemented for HID yet."
#endif
};
