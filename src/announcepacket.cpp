//
// Created by prise on 6/28/20.
//

#include "include/udp/announcepacket.h"
#include <boost/asio.hpp>
#include <random>
#include "utilities.h"

namespace cocktorrent::udp {

AnnouncePacket::AnnouncePacket(int64_t connectionId,
                               const TorrentBaseFileInfo &file_info,
                               uint16_t port)
    : AnnouncePacket(connectionId, file_info.info_hash(),
                     file_info.downloaded(), file_info.left(),
                     file_info.uploaded(),
                     TorrentBaseFileInfo::ToInteger(file_info.state()), port) {}

AnnouncePacket::AnnouncePacket(int64_t connectionId,
                               const AnnouncePacket::InfoHashType &infoHash,
                               int64_t downloaded, int64_t left,
                               int64_t uploaded, int32_t event, uint16_t port)
    : connectionID_{connectionId},
      info_hash_{infoHash},
      peerID_{},
      downloaded_{downloaded},
      left_{left},
      uploaded_{uploaded},
      event_{event},
      ip_address_{0},
      num_want_{-1},
      port_{port},
      extensions_{0} {
  std::uniform_int_distribution<char> distribution;
  key_ = distribution(util::generator);
  std::generate(std::begin(peerID_), std::end(peerID_),
                [&distribution]() { return distribution(util::generator); });
  transactionID_ = distribution(util::generator);
  util::Put(buffer_, connectionID_, action, transactionID_, info_hash_,
            peerID_, downloaded_, left_, uploaded_, event_, ip_address_, key_,
            num_want_, port_, extensions_);
}

int64_t AnnouncePacket::connectionID() const { return connectionID_; }

int32_t AnnouncePacket::transactionID() const { return transactionID_; }

const AnnouncePacket::InfoHashType &AnnouncePacket::info_hash() const {
  return info_hash_;
}

const AnnouncePacket::PeerIDType &AnnouncePacket::peerID() const {
  return peerID_;
}

int64_t AnnouncePacket::downloaded() const { return downloaded_; }

int64_t AnnouncePacket::left() const { return left_; }

int64_t AnnouncePacket::uploaded() const { return uploaded_; }

int32_t AnnouncePacket::event() const { return event_; }

uint32_t AnnouncePacket::ip_address() const { return ip_address_; }

uint32_t AnnouncePacket::key() const { return key_; }

int32_t AnnouncePacket::num_want() const { return num_want_; }

uint16_t AnnouncePacket::port() const { return port_; }

uint16_t AnnouncePacket::extensions() const { return extensions_; }

const AnnouncePacket::BufferType &AnnouncePacket::buffer() const {
  return buffer_;
}
}  // namespace cocktorrent::udp
