//
// Created by prise on 6/28/20.
//
#include <details/utils/utility.hpp>
#include <libbencode/bencode.hpp>
#include <libtracker/packets/udp/connect.hpp>

namespace opentorrent::packets::udp {
Connect::Connect() {
  std::uniform_int_distribution<uint32_t> distribution;
  transactionID_ = distribution(details::utils::generator);
  details::utils::Put(buffer_, connectionID_, actionID_, transactionID_);
}
int64_t Connect::connectionID() { return connectionID_; }
int32_t Connect::actionID() { return actionID_; }
int32_t Connect::transactionID() const { return transactionID_; }
const Connect::BufferType& Connect::buffer() const {
  return buffer_;
}
}  // namespace cocktorrent::udp
