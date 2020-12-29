//
// Created by prise on 6/28/20.
//

#ifndef OPENTORRENT_RESPONSE_CONNECT_H
#define OPENTORRENT_RESPONSE_CONNECT_H
#include <boost/asio.hpp>
#include <cinttypes>

namespace opentorrent::packets::udp {
class ResponseConnect {
 public:
  ResponseConnect(const boost::asio::const_buffer &buf, int32_t transactionID);
  [[nodiscard]] int64_t connectionID() const;

 private:
  int32_t action_;
  int32_t transactionID_{};
  int64_t connectionID_{};
};
}  // namespace opentorrent::packets::udp

#endif  // OPENTORRENT_RESPONSE_CONNECT_H
