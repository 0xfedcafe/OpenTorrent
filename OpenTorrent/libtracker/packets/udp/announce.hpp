//
// Created by prise on 6/28/20.
//

#ifndef OPENTORRENT_ANNOUNCE_H
#define OPENTORRENT_ANNOUNCE_H
#include <boost/asio.hpp>
#include <cinttypes>
#include <libtracker/packets/udp/response-connect.hpp>

namespace opentorrent::packets::udp {
class Announce {
 public:
  static constexpr int32_t action_ = 1;

  using BufferType = std::array<char, 100>;

  Announce(int64_t connectionId, std::array<char, 20> infoHash,
           int64_t downloaded, int64_t left, int64_t uploaded, int32_t event,
           uint32_t ipAddress, int32_t numWant, uint16_t port,
           uint16_t extensions);

  [[nodiscard]] int32_t transactionID() const;

  [[nodiscard]] const BufferType &buffer() const;

 private:
  int64_t connectionID_;
  int32_t transactionID_;
  std::array<char, 20> infoHash_;
  std::array<char, 20> peerID_{};
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
}  // namespace opentorrent::packets::udp
#endif  // OPENTORRENT_ANNOUNCE_H
