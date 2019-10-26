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
#include "defaultBowlerComs.hpp"
#include "mockBowlerServer.hpp"
#include "mockPacket.hpp"
#include "noopPacket.hpp"
#include <unity.h>

using namespace bowlerserver;

#define SETUP_BOWLER_COMS                                                                          \
  MockBowlerServer<N> *server = new MockBowlerServer<N>();                                         \
  DefaultBowlerComs<N> coms {                                                                      \
    std::unique_ptr<MockBowlerServer<N>>(server)                                                   \
  }

#define MAKE_PACKET(typeName, args...) coms.addPacket(std::shared_ptr<typeName>(new typeName(args)))

template <std::size_t N>
static void assertReceiveSend(MockBowlerServer<N> *server,
                              DefaultBowlerComs<N> &coms,
                              const std::array<std::uint8_t, N> &receive,
                              const std::array<std::uint8_t, N> &send) {
  server->readsToSend.push(receive);
  coms.loop();
  TEST_ASSERT_EQUAL_UINT8_ARRAY(send.data(), server->writesReceived.front().data(), N);
  server->writesReceived.pop();
}

template <std::size_t N> void receive_seqnum_0() {
  SETUP_BOWLER_COMS;
  MAKE_PACKET(NoopPacket, 2, true);

  // Send SeqNum 0 first (expected). Should ACK 0.
  assertReceiveSend(server, coms, {2, 0, 1}, {2, 0, 0});
}

template <std::size_t N> void receive_seqnum_1() {
  SETUP_BOWLER_COMS;
  MAKE_PACKET(NoopPacket, 2, true);

  // Send SeqNum 1 first (not expected). Should ACK 1.
  assertReceiveSend(server, coms, {2, 1, 0}, {2, 1, 1});
}

template <std::size_t N> void receive_seqnums_0_1() {
  SETUP_BOWLER_COMS;
  MAKE_PACKET(NoopPacket, 2, true);

  // Send SeqNum 0 first (expected). Should ACK 0.
  assertReceiveSend(server, coms, {2, 0, 1}, {2, 0, 0});

  // Send SeqNum 1 (expected). Should ACK 1.
  assertReceiveSend(server, coms, {2, 1, 0}, {2, 1, 1});
}

template <std::size_t N> void receive_seqnums_0_0() {
  SETUP_BOWLER_COMS;
  MAKE_PACKET(NoopPacket, 2, true);

  // Send SeqNum 0 first (expected). Should ACK 0.
  assertReceiveSend(server, coms, {2, 0, 1}, {2, 0, 0});

  // Send SeqNum 0 (not expected). Should ACK 0.
  assertReceiveSend(server, coms, {2, 0, 1}, {2, 0, 0});
}

template <std::size_t N> void receive_seqnums_0_1_1() {
  SETUP_BOWLER_COMS;
  MAKE_PACKET(NoopPacket, 2, true);

  // Send SeqNum 0 first (expected). Should ACK 0.
  assertReceiveSend(server, coms, {2, 0, 1}, {2, 0, 0});

  // Send SeqNum 1 (expected). Should ACK 1.
  assertReceiveSend(server, coms, {2, 1, 0}, {2, 1, 1});

  // Send SeqNum 1 (not expected). Should ACK 1.
  assertReceiveSend(server, coms, {2, 1, 1}, {2, 1, 1});
}

template <std::size_t N> void attach_server_management_packet_id() {
  SETUP_BOWLER_COMS;
  TEST_ASSERT_EQUAL_INT(BOWLER_ERROR, MAKE_PACKET(NoopPacket, SERVER_MANAGEMENT_PACKET_ID));
}

template <std::size_t N> void unreliable() {
  SETUP_BOWLER_COMS;
  MAKE_PACKET(NoopPacket, 2, false);

  // Unreliable should just echo the header
  assertReceiveSend(server, coms, {2, 0, 0}, {2, 0, 0});
  assertReceiveSend(server, coms, {2, 1, 1}, {2, 1, 1});
}

template <std::size_t N> void packet_does_not_get_header_data() {
  SETUP_BOWLER_COMS;
  std::shared_ptr<MockPacket> mockPacket(new MockPacket(2, false));
  coms.addPacket(mockPacket);

  // The first (and only) payload should be all 0's because the header data should be stripped
  assertReceiveSend(server, coms, {2, 1, 1}, {2, 1, 1});

  std::array<std::uint8_t, DEFAULT_PACKET_SIZE - HEADER_LENGTH> expected{};
  TEST_ASSERT_EQUAL_UINT8_ARRAY(expected.data(), mockPacket->payloads[0].data(), expected.size());
}

template <std::size_t N> void get_all_packet_ids() {
  SETUP_BOWLER_COMS;
  coms.addPacket(std::shared_ptr<MockPacket>(new MockPacket(2, false)));
  coms.addPacket(std::shared_ptr<MockPacket>(new MockPacket(3, false)));

  auto ids = coms.getAllPacketIDs();
  std::array<std::uint8_t, 2> expected{2, 3};
  TEST_ASSERT_EQUAL_UINT8_ARRAY(expected.data(), ids.data(), expected.size());
}

template <std::size_t N> void remove_packet() {
  SETUP_BOWLER_COMS;
  coms.addPacket(std::shared_ptr<MockPacket>(new MockPacket(2, false)));
  coms.addPacket(std::shared_ptr<MockPacket>(new MockPacket(3, false)));

  coms.removePacket(2);

  auto ids = coms.getAllPacketIDs();
  std::array<std::uint8_t, 1> expected{3};
  TEST_ASSERT_EQUAL_UINT8_ARRAY(expected.data(), ids.data(), expected.size());
}

void setup() {
  delay(2000);
  UNITY_BEGIN();
  RUN_TEST(receive_seqnum_0<DEFAULT_PACKET_SIZE>);
  RUN_TEST(receive_seqnum_1<DEFAULT_PACKET_SIZE>);
  RUN_TEST(receive_seqnums_0_1<DEFAULT_PACKET_SIZE>);
  RUN_TEST(receive_seqnums_0_0<DEFAULT_PACKET_SIZE>);
  RUN_TEST(receive_seqnums_0_1_1<DEFAULT_PACKET_SIZE>);
  RUN_TEST(attach_server_management_packet_id<DEFAULT_PACKET_SIZE>);
  RUN_TEST(unreliable<DEFAULT_PACKET_SIZE>);
  RUN_TEST(packet_does_not_get_header_data<DEFAULT_PACKET_SIZE>);
  RUN_TEST(get_all_packet_ids<DEFAULT_PACKET_SIZE>);
  RUN_TEST(remove_packet<DEFAULT_PACKET_SIZE>);
  UNITY_END();
}

void loop() {
}
