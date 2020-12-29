//
// Created by prise on 6/28/20.
//

#include <details/utils/utility.hpp>
#include <liblogger/logger.hpp>
#include <libtracker/packets/udp/response-announce.hpp>

namespace opentorrent::packets::udp {
ResponseAnnounce::ResponseAnnounce(const boost::asio::const_buffer& buffer,
                                   int32_t transactionID) {
  if (buffer.size() < 20) {
    Logger::get_instance()->Error("Bad packet");
    throw std::runtime_error{"Bad packet"};
  }
  std::string_view buff_view{static_cast<const char*>(buffer.data()),
                             buffer.size()};
  action_ =
      details::utils::FromNetworkCharSequence<int32_t>(buff_view.substr(0, 4));
  buff_view.remove_prefix(4);

  transactionID_ =
      details::utils::FromNetworkCharSequence<int32_t>(buff_view.substr(0, 4));
  buff_view.remove_prefix(4);

  if (transactionID_ != transactionID) {
    Logger::get_instance()->Error("ResponseAnnounce: Transaction ID mismatch");
    throw std::runtime_error{"ResponseAnnounce: Transaction ID mismatch"};
  }

  interval_ =
      details::utils::FromNetworkCharSequence<int32_t>(buff_view.substr(0, 4));
  buff_view.remove_prefix(4);

  leechers_ =
      details::utils::FromNetworkCharSequence<int32_t>(buff_view.substr(0, 4));
  buff_view.remove_prefix(4);

  seeders_ =
      details::utils::FromNetworkCharSequence<int32_t>(buff_view.substr(0, 4));
  buff_view.remove_prefix(4);

  while (buff_view.size() >= 6) {
    auto to_return = Seed{details::utils::FromNetworkCharSequence<uint32_t>(
                              buff_view.substr(0, 4)),
                          details::utils::FromNetworkCharSequence<uint16_t>(
                              buff_view.substr(4, 2))};
    buff_view.remove_prefix(6);
    if (to_return.port || to_return.ip) {
      peers_.push_back(to_return);
    }
  }
}

const std::vector<ResponseAnnounce::Seed>& ResponseAnnounce::peers() const {
  return peers_;
}
}  // namespace opentorrent::packets::udp
