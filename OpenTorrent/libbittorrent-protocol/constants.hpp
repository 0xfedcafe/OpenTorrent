//
// Created by vyacheslav on 1/2/21.
//

#ifndef OPENTORRENT_CONSTANTS_HPP
#define OPENTORRENT_CONSTANTS_HPP

#include <libbittorrent-protocol/view.hpp>

namespace opentorrent {
[[maybe_unused]] inline const StaticString<20> peer_id = [] {
  StaticString<20> peer_id = {{"-CK0001-"}};
  constexpr auto sz = sizeof("-CK0001-") / sizeof("-CK0001-"[0]);
  std::generate(peer_id.begin() + sz, peer_id.end(), [] {
    return details::utils::random<StaticString<20>::value_type>();
  });
  return peer_id;
}();

[[maybe_unused]] inline const int32_t listen_port = 6881;
}

#endif  // OPENTORRENT_CONSTANTS_HPP
