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
#include "bowlerUdpServer.hpp"
#include "defaultBowlerComs.hpp"
#include "noopPacket.hpp"
#include <Arduino.h>
#include <Esp32WifiManager.h>

namespace bowlerserver {
template <std::size_t N> class BowlerComsController {
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

  BowlerComs<N> &getComs() {
    return coms;
  }

  protected:
  void setup() {
    if (state != startup) {
      return;
    }

    state = waitForConnection;

#if defined(USE_WIFI)
    manager.setupAP();
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
  DefaultBowlerComs<N> coms{std::unique_ptr<UDPServer<N>>(new UDPServer<N>())};
#elif defined(USE_HID)
#error "BowlerServerController not implemented for HID yet."
#endif
};
} // namespace bowlerserver
