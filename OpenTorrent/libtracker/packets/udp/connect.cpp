//
// Created by prise on 6/28/20.
//
#include <details/utils/net/utility.hpp>
#include <libbencode/bencode.hpp>
#include <libtracker/packets/udp/connect.hpp>

namespace opentorrent::packets::udp {
Connect::Connect() {
  std::uniform_int_distribution<uint32_t> distribution;
  transactionID_ = details::utils::net::TransactionID();
  details::utils::Put(buffer_, connectionID_, actionID_, transactionID_);
}
int32_t Connect::transactionID() const { return transactionID_; }
const Connect::BufferType& Connect::buffer() const { return buffer_; }
}  // namespace opentorrent::packets::udp
