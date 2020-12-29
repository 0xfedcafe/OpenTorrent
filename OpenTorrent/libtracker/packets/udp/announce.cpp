//
// Created by prise on 6/28/20.
//

#include "announce.hpp"
#include <details/utils/net/utility.hpp>
#include <details/utils/utility.hpp>
#include <random>

namespace opentorrent::packets::udp {

Announce::Announce(int64_t connectionId, std::array<char, 20> infoHash,
                   int64_t downloaded, int64_t left, int64_t uploaded,
                   int32_t event, uint32_t ipAddress, int32_t numWant,
                   uint16_t port, uint16_t extensions)
    : connectionID_(connectionId),
      infoHash_(infoHash),
      downloaded_(downloaded),
      left_(left),
      uploaded_(uploaded),
      event_(event),
      ipAddress_(ipAddress),
      numWant_(numWant),
      port_(port),
      extensions_(extensions) {
  transactionID_ = details::utils::net::TransactionID();
  key_ = details::utils::random<uint32_t>();
  std::generate(peerID_.begin(), peerID_.end(),
                []() { return details::utils::random<char>(); });
  details::utils::Put(buffer_, connectionID_, action_, transactionID_,
                      infoHash_, peerID_, downloaded_, left_, uploaded_, event_,
                      ipAddress_, key_, numWant_, port_, extensions_);
}

int32_t Announce::transactionID() const { return transactionID_; }

const Announce::BufferType &Announce::buffer() const { return buffer_; }
}  // namespace opentorrent::packets::udp
