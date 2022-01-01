//
// Created by vyacheslav on 12/29/20.
//

#ifndef OPENTORRENT_UTILITY_HPP
#define OPENTORRENT_UTILITY_HPP
#include <boost/asio.hpp>
#include <details/utils/utility.hpp>
#include <random>

namespace details::utils::net {

[[nodiscard]] inline bool IsUdp(::std::string_view url) {
  return url.find("udp://") == 0;
}

[[nodiscard]] inline ::boost::asio::ip::udp::resolver::results_type
GetUDPEndPoints(::std::string_view url, ::boost::asio::io_context &io_context) {
  std::string_view domain;
  std::string_view port;
  if (IsUdp(url)) {
    url.remove_prefix(sizeof("udp://") - 1);
    auto start_port = url.find(':');
    domain = url.substr(0, start_port);
    auto sv = url.begin() + start_port + 1;
    auto it = sv;
    while (std::isdigit(*it)) ++it;
    port = url.substr(start_port + 1, it - sv);
  } else {
    return {};
  }

  ::boost::asio::ip::udp::resolver resolver{io_context};

  return resolver.resolve(domain, port);
}

[[nodiscard]] inline int32_t TransactionID() {
  return ::details::utils::random<int32_t>();
}
}  // namespace details::utils::net

#endif  // OPENTORRENT_UTILITY_HPP
