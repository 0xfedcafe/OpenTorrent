//
// Created by prise on 7/2/20.
//

#ifndef OPENTORRENT_PEERHANDSHAKE_H
#define OPENTORRENT_PEERHANDSHAKE_H
#include <boost/asio.hpp>
#include <string_view>

namespace opentorrent::peer::tcp {
class PeerHandshake {
 public:
  static constexpr std::string_view protocol_id = "BitTorrent protocol";
  static constexpr char pstrlen = 19;
  static constexpr int64_t reserved = 0;

  PeerHandshake() = delete;
  explicit PeerHandshake(const std::array<char, 20> &info_hash);

  [[nodiscard]] const boost::asio::streambuf &buffer() const { return buffer_; }
  [[nodiscard]] const std::array<char, 20> &info_hash() const {
    return info_hash_;
  }
  [[nodiscard]] const std::array<char, 20> &peer_id() { return peer_id_; }

 private:
  std::array<char, 20> info_hash_;
  std::array<char, 20> peer_id_ = {{"-CK0001-"}};
  boost::asio::streambuf buffer_;
};
}  // namespace cocktorrent::peer::tcp

#endif  // OPENTORRENT_PEERHANDSHAKE_H
