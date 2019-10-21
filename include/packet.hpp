#pragma once

#include <cstdint>

template <std::size_t N> class Packet {
  public:
  Packet(std::uint8_t iid, bool iisReliable = false) : id(iid), m_isReliable(iisReliable) {
  }

  virtual ~Packet() = default;

  virtual std::int32_t event(std::array<std::uint8_t, N> payload) = 0;

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
