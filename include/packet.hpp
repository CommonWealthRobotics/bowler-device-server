#pragma once

#include <cstdint>

template <std::size_t N> class Packet {
  public:
  Packet(std::uint8_t iid) : id(iid) {
  }

  virtual ~Packet() = default;

  virtual std::int32_t event(std::array<std::uint8_t, N> payload) = 0;

  std::uint8_t getId() const {
    return id;
  }

  protected:
  std::uint8_t id;
};
