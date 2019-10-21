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
#include "bowlerComs.hpp"
#include "mockBowlerServer.hpp"
#include "noopPacket.hpp"
#include <unity.h>

#define SETUP_BOWLER_COMS                                                                          \
  MockBowlerServer<N> *server = new MockBowlerServer<N>();                                         \
  BowlerComs<N> coms {                                                                             \
    std::unique_ptr<MockBowlerServer<N>>(server)                                                   \
  }

#define MAKE_PACKET(typeName, args...) coms.addPacket(std::unique_ptr<typeName>(new typeName(args)))

template <std::size_t N>
static void assertReceiveSend(MockBowlerServer<N> *server,
                              BowlerComs<N> &coms,
                              const std::array<std::uint8_t, N> &receive,
                              const std::array<std::uint8_t, N> &send) {
  server->readsToSend.push(receive);
  coms.loop();
  TEST_ASSERT_EQUAL_UINT8_ARRAY(send.data(), server->writesReceived.front().data(), N);
  server->writesReceived.pop();
}

template <std::size_t N> void receive_seqnum_0() {
  SETUP_BOWLER_COMS;
  MAKE_PACKET(NoopPacket<N>, 1);

  // Send SeqNum 0 first (expected). Should ACK 0.
  assertReceiveSend(server, coms, {1, 0, 1}, {1, 0, 0});
}

template <std::size_t N> void receive_seqnum_1() {
  SETUP_BOWLER_COMS;
  MAKE_PACKET(NoopPacket<N>, 1);

  // Send SeqNum 1 first (not expected). Should ACK 1.
  assertReceiveSend(server, coms, {1, 1, 0}, {1, 1, 1});
}

template <std::size_t N> void receive_seqnums_0_1() {
  SETUP_BOWLER_COMS;
  MAKE_PACKET(NoopPacket<N>, 1);

  // Send SeqNum 0 first (expected). Should ACK 0.
  assertReceiveSend(server, coms, {1, 0, 1}, {1, 0, 0});

  // Send SeqNum 1 (expected). Should ACK 1.
  assertReceiveSend(server, coms, {1, 1, 0}, {1, 1, 1});
}

template <std::size_t N> void receive_seqnums_0_0() {
  SETUP_BOWLER_COMS;
  MAKE_PACKET(NoopPacket<N>, 1);

  // Send SeqNum 0 first (expected). Should ACK 0.
  assertReceiveSend(server, coms, {1, 0, 1}, {1, 0, 0});

  // Send SeqNum 0 (not expected). Should ACK 0.
  assertReceiveSend(server, coms, {1, 0, 1}, {1, 0, 0});
}

template <std::size_t N> void receive_seqnums_0_1_1() {
  SETUP_BOWLER_COMS;
  MAKE_PACKET(NoopPacket<N>, 1);

  // Send SeqNum 0 first (expected). Should ACK 0.
  assertReceiveSend(server, coms, {1, 0, 1}, {1, 0, 0});

  // Send SeqNum 1 (expected). Should ACK 1.
  assertReceiveSend(server, coms, {1, 1, 0}, {1, 1, 1});

  // Send SeqNum 1 (not expected). Should ACK 1.
  assertReceiveSend(server, coms, {1, 1, 1}, {1, 1, 1});
}

void setup() {
  delay(2000);
  UNITY_BEGIN();
  RUN_TEST(receive_seqnum_0<DEFAULT_PACKET_SIZE>);
  RUN_TEST(receive_seqnum_1<DEFAULT_PACKET_SIZE>);
  RUN_TEST(receive_seqnums_0_1<DEFAULT_PACKET_SIZE>);
  RUN_TEST(receive_seqnums_0_0<DEFAULT_PACKET_SIZE>);
  RUN_TEST(receive_seqnums_0_1_1<DEFAULT_PACKET_SIZE>);
  UNITY_END();
}

void loop() {
}
