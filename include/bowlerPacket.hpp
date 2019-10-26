#pragma once

#include <array>
#include <cstdint>

namespace bowlerserver {
class Packet {
  public:
  Packet(std::uint8_t iid, bool iisReliable = false) : id(iid), m_isReliable(iisReliable) {
  }

  virtual ~Packet() = default;

  /**
   * Processes the payload (read from it, do something, write back into it).
   *
   * @param payload The payload (not including header data).
   * @return `1` on success or BOWLER_ERROR on error.
   */
  virtual std::int32_t event(std::uint8_t *payload) = 0;

  std::uint8_t getId() const {
    return id;
  }

  bool isReliable() const {
    return m_isReliable;
  }

  protected:
  std::uint8_t id;
  bool m_isReliable;
};
} // namespace bowlerserver
