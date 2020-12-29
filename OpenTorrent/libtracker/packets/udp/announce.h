//
// Created by prise on 6/28/20.
//

#ifndef COCKTORRENT_ANNOUNCE_H
#define COCKTORRENT_ANNOUNCE_H
#include <libtracker/packets/udp/response-connect.h>
#include <boost/asio.hpp>
#include <cinttypes>

namespace opentorrent::packets::udp {
class Announce {
 public:
  static constexpr int32_t action_ = 1;

  using BufferType = std::array<char, 100>;

  Announce(int64_t connectionId, std::array<char, 20> infoHash,
                 std::array<char, 20> peerId, int64_t downloaded, int64_t left,
                 int64_t uploaded, int32_t event, uint32_t ipAddress,
                 uint32_t key, int32_t numWant, uint16_t port,
                 uint16_t extensions);

  [[nodiscard]] int64_t connectionID() const;

  [[nodiscard]] int32_t transactionID() const;

  [[nodiscard]] const std::array<char, 20> &infoHash() const;

  [[nodiscard]] const std::array<char, 20> &peerID() const;

  [[nodiscard]] int64_t downloaded() const;

  [[nodiscard]] int64_t left() const;

  [[nodiscard]] int64_t uploaded() const;

  [[nodiscard]] int32_t event() const;

  [[nodiscard]] uint32_t ipAddress() const;

  [[nodiscard]] uint32_t key() const;

  [[nodiscard]] int32_t numWant() const;

  [[nodiscard]] uint16_t port() const;

  [[nodiscard]] uint16_t extensions() const;

  [[nodiscard]] const BufferType &buffer() const;

 private:
  int64_t connectionID_;
  int32_t transactionID_;
  std::array<char, 20> infoHash_;
  std::array<char, 20> peerID_;
  int64_t downloaded_;
  int64_t left_;
  int64_t uploaded_;
  int32_t event_;
  uint32_t ipAddress_;
  uint32_t key_;
  int32_t numWant_;
  uint16_t port_;
  uint16_t extensions_;
  BufferType buffer_{};
};
}  // namespace opentorrent::udp
#endif  // COCKTORRENT_ANNOUNCE_H
