//
// Created by prise on 6/28/20.
//

#include "announce.hpp"
#include <boost/asio.hpp>
#include <details/utils/utility.hpp>
#include <random>

namespace opentorrent::packets::udp {

Announce::Announce(int64_t connectionId,
                               std::array<char, 20> infoHash,
                               std::array<char, 20> peerId, int64_t downloaded,
                               int64_t left, int64_t uploaded, int32_t event,
                               uint32_t ipAddress, uint32_t key,
                               int32_t numWant, uint16_t port,
                               uint16_t extensions)
    : connectionID_(connectionId),
      infoHash_(infoHash),
      peerID_(peerId),
      downloaded_(downloaded),
      left_(left),
      uploaded_(uploaded),
      event_(event),
      ipAddress_(ipAddress),
      key_(key),
      numWant_(numWant),
      port_(port),
      extensions_(extensions) {
  std::uniform_int_distribution<uint32_t> distribution;
  transactionID_ = distribution(details::utils::generator);
  details::utils::Put(buffer_, connectionID_, action_, transactionID_, infoHash_, peerID_,
            downloaded_, left_, uploaded_, event_, ipAddress_, key_, numWant_,
            port_, extensions_);
}

int64_t Announce::connectionID() const { return connectionID_; }

int32_t Announce::transactionID() const { return transactionID_; }

const std::array<char, 20> &Announce::infoHash() const {
  return infoHash_;
}

const std::array<char, 20> &Announce::peerID() const { return peerID_; }

int64_t Announce::downloaded() const { return downloaded_; }

int64_t Announce::left() const { return left_; }

int64_t Announce::uploaded() const { return uploaded_; }

int32_t Announce::event() const { return event_; }

uint32_t Announce::ipAddress() const { return ipAddress_; }

uint32_t Announce::key() const { return key_; }

int32_t Announce::numWant() const { return numWant_; }

uint16_t Announce::port() const { return port_; }

uint16_t Announce::extensions() const { return extensions_; }

const Announce::BufferType &Announce::buffer() const { return buffer_; }
}  // namespace cocktorrent::udp
