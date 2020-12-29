//
// Created by prise on 6/28/20.
//

#ifndef OPENTORRENT_CONNECT_H
#define OPENTORRENT_CONNECT_H

#include <boost/asio.hpp>

namespace opentorrent::packets::udp {
class Connect {
 public:
  using BufferType = std::array<char, 16>;

  Connect();
  static int64_t connectionID();
  static int32_t actionID();
  [[nodiscard]] int32_t transactionID() const;
  [[nodiscard]] const BufferType& buffer() const;

 private:
  static constexpr int64_t connectionID_ = 0x41727101980;
  static constexpr int32_t actionID_ = 0;
  int32_t transactionID_{};
  BufferType buffer_{};
};
}  // namespace opentorrent::udp
#endif  // OPENTORRENT_CONNECT_H
