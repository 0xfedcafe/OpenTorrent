//
// Created by prise on 6/28/20.
//

#ifndef COCKTORRENT_RESPONSE_ANNOUNCE_H
#define COCKTORRENT_RESPONSE_ANNOUNCE_H

#include <boost/asio.hpp>
#include <cinttypes>
#include <vector>

namespace opentorrent::packets::udp {
class ResponseAnnounce {
 public:
  ResponseAnnounce() = delete;
  ResponseAnnounce(const boost::asio::const_buffer& buffer,
                         int32_t transactionID);
  struct Seed {
    uint32_t ip;
    uint16_t port;
  };

  [[nodiscard]] int32_t action() const;

  [[nodiscard]] int32_t transactionId() const;

  [[nodiscard]] int32_t interval() const;

  [[nodiscard]] int32_t leechers() const;

  [[nodiscard]] int32_t seeders() const;

  [[nodiscard]] const std::vector<Seed>& peers() const;

 private:
  int32_t action_{};
  int32_t transactionID_{};
  int32_t interval_{};
  int32_t leechers_{};
  int32_t seeders_{};
  std::vector<Seed> peers_;
};
}  // namespace opentorrent::udp
#endif  // COCKTORRENT_RESPONSE_ANNOUNCE_H
