//
// Created by prise on 6/28/20.
//

#include <liblogger/logger.h>
#include <libtracker/packets/udp/response-connect.h>
#include <details/utils/utility.hpp>

namespace opentorrent::packets::udp {

int32_t ResponseConnect::action() const { return action_; }
int32_t ResponseConnect::transactionID() const { return transactionID_; }
int64_t ResponseConnect::connectionID() const { return connectionID_; }
ResponseConnect::ResponseConnect(
    const boost::asio::const_buffer& buf, int32_t transactionID) {
  std::string_view data{static_cast<const char*>(buf.data()), buf.size()};

  this->action_ = details::utils::FromNetworkCharSequence<int32_t>(data.substr(0, 4));

  data.remove_prefix(4);

  this->transactionID_ =
      details::utils::FromNetworkCharSequence<int32_t>(data.substr(0, 4));

  data.remove_prefix(4);

  this->connectionID_ =
      details::utils::FromNetworkCharSequence<int64_t>(data.substr(0, 8));

  if (transactionID_ != transactionID) {
    Logger::get_instance()->Error(
        "ResponseConnect: TransactionID mismatch " +
        std::to_string(transactionID) + " vs " +
        std::to_string(transactionID_));
    throw std::logic_error{"ResponseConnect: TransactionID mismatch " +
                           std::to_string(transactionID) + " vs " +
                           std::to_string(transactionID_)};
  }
  if (action_ != 0) {
    Logger::get_instance()->Error("ResponseConnect: Action mismatch" +
                                  std::to_string(0) + " vs " +
                                  std::to_string(action_));
    throw std::logic_error{"ResponseConnect: Action mismatch" +
                           std::to_string(0) + " vs " +
                           std::to_string(action_)};
  }
}
}  // namespace cocktorrent::udp