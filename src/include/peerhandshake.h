//
// Created by prise on 7/2/20.
//

#ifndef COCKTORRENT_PEERHANDSHAKE_H
#define COCKTORRENT_PEERHANDSHAKE_H
#include <utilities.h>
#include <boost/asio.hpp>
#include <string_view>

namespace cocktorrent::peer::tcp {
class PeerHandshake {
 public:
  static constexpr std::string_view protocol_id = "BitTorrent protocol";
  static constexpr char pstrlen = protocol_id.size();
  static constexpr int64_t reserved = 0;

  using InfoHashType = std::array<char, 20>;
  using PeerIdType = std::array<char, 20>;
  using BufferType =
  std::array<char, sizeof(pstrlen) + pstrlen + sizeof(reserved) +
                   sizeof(InfoHashType) +  sizeof(PeerIdType)>;

  PeerHandshake() = delete;
  explicit PeerHandshake(const InfoHashType &info_hash)
      : info_hash_{info_hash} {
    util::Put(buffer_, pstrlen, protocol_id, reserved, info_hash_, peer_id_);
  }

  explicit PeerHandshake(const BufferType &buf) {
    util::Get(buf.data() + sizeof(pstrlen) + pstrlen + sizeof(reserved),
              info_hash_, peer_id_);
  }

  [[nodiscard]] const BufferType &buffer() const noexcept { return buffer_; }
  [[nodiscard]] const InfoHashType &info_hash() const noexcept { return info_hash_; }
  [[nodiscard]] const PeerIdType &peer_id() noexcept { return peer_id_; }

 private:
  InfoHashType info_hash_;
  PeerIdType peer_id_ = {{"-CK0001-12341243909"}};
  BufferType buffer_{};
};
}  // namespace cocktorrent::peer::tcp

#endif  // COCKTORRENT_PEERHANDSHAKE_H
