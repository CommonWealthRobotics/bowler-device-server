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
#include "util.hpp"
#include <WiFi.h>
#include <WiFiUdp.h>
#include <functional>

#define BOWLER_SERVER_UDP_PORT 1866

/**
 * A BowlerServer which uses UDP. Listens on port BOWLER_SERVER_UDP_PORT.
 */
template <std::size_t N> class UDPServer : public BowlerServer<N> {
  public:
  UDPServer() {
    event = WiFi.onEvent(std::bind(&UDPServer::callback, this, std::placeholders::_1));
  }

  virtual ~UDPServer() {
    WiFi.removeEvent(event);
  }

  std::int32_t write(std::array<std::uint8_t, N> payload) override {
    if (!connected) {
      errno = ENOTCONN;
      return BOWLER_ERROR;
    }

    if (!udp.beginPacket()) {
      // beginPacket will set errno
      return BOWLER_ERROR;
    }

    udp.write(payload.data(), payload.size());
    if (!udp.endPacket()) {
      // endPacket will set errno
      return BOWLER_ERROR;
    }

    return 1;
  }

  std::int32_t read(std::array<std::uint8_t, N> &payload) override {
    if (!connected) {
      errno = ENOTCONN;
      return BOWLER_ERROR;
    }

    udp.read(payload.data(), payload.size());
    return 1;
  }

  std::int32_t isDataAvailable(bool &available) override {
    if (!connected) {
      errno = ENOTCONN;
      available = false;
      return BOWLER_ERROR;
    }

    if (!udp.parsePacket()) {
      available = false;
      // parsePacket will set errno
      return BOWLER_ERROR;
    }

    available = udp.available() > 0;
    return 1;
  }

  protected:
  void callback(WiFiEvent_t event) {
    switch (event) {
    case SYSTEM_EVENT_STA_GOT_IP:
      // ESP32 station got IP from connected AP
      udp.begin(WiFi.localIP(), BOWLER_SERVER_UDP_PORT);
      connected = true;
      break;

    case SYSTEM_EVENT_STA_DISCONNECTED:
      // ESP32 station disconnected from AP
      connected = false;
      break;

    case SYSTEM_EVENT_AP_STACONNECTED:
      // A station connected to ESP32 soft-AP
      if (!connected) {
        udp.begin(WiFi.softAPIP(), BOWLER_SERVER_UDP_PORT);
        connected = true;
      }
      break;

    default:
      break;
    }
  }

  private:
  WiFiUDP udp;
  wifi_event_id_t event;
  bool connected{false};
};
