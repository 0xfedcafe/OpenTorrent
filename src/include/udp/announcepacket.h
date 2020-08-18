//
// Created by prise on 6/28/20.
//

#ifndef COCKTORRENT_ANNOUNCEPACKET_H
#define COCKTORRENT_ANNOUNCEPACKET_H

#include <torrentbasefileinfo.h>
#include <boost/asio.hpp>
#include <cinttypes>

namespace cocktorrent::udp {
class AnnouncePacket {
 public:
  static constexpr int32_t action = 1;
  static constexpr uint16_t default_port = 6889;

  using BufferType = std::array<char, 100>;
  using InfoHashType = TorrentBaseFileInfo::InfoHashType;
  using PeerIDType = std::array<char, 20>;

  AnnouncePacket(int64_t connectionId, const TorrentBaseFileInfo &file_info,
                 uint16_t port);

  AnnouncePacket(int64_t connectionId, const InfoHashType &infoHash,
                 int64_t downloaded, int64_t left, int64_t uploaded,
                 int32_t event, uint16_t port);

  [[nodiscard]] int64_t connectionID() const;

  [[nodiscard]] int32_t transactionID() const;

  [[nodiscard]] const InfoHashType &info_hash() const;

  [[nodiscard]] const PeerIDType &peerID() const;

  [[nodiscard]] int64_t downloaded() const;

  [[nodiscard]] int64_t left() const;

  [[nodiscard]] int64_t uploaded() const;

  [[nodiscard]] int32_t event() const;

  [[nodiscard]] uint32_t ip_address() const;

  [[nodiscard]] uint32_t key() const;

  [[nodiscard]] int32_t num_want() const;

  [[nodiscard]] uint16_t port() const;

  [[nodiscard]] uint16_t extensions() const;

  [[nodiscard]] const BufferType &buffer() const;

 private:
  int64_t connectionID_;
  int32_t transactionID_;
  InfoHashType info_hash_;
  PeerIDType peerID_;
  int64_t downloaded_;
  int64_t left_;
  int64_t uploaded_;
  int32_t event_;
  uint32_t ip_address_;
  uint32_t key_;
  int32_t num_want_;
  uint16_t port_;
  uint16_t extensions_;
  BufferType buffer_{};
};
}  // namespace cocktorrent::udp
#endif  // COCKTORRENT_ANNOUNCEPACKET_H
