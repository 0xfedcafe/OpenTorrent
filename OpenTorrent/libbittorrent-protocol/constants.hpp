//
// Created by vyacheslav on 1/2/21.
//

#ifndef OPENTORRENT_CONSTANTS_HPP
#define OPENTORRENT_CONSTANTS_HPP

#include <spdlog/spdlog.h>
#include <libbittorrent-protocol/view.hpp>
#include "debug.h"
#include "details/utils/utility.hpp"
namespace opentorrent {

[[maybe_unused]] inline const int32_t listen_port = 6881;

[[maybe_unused]] inline const StaticString<20> peer_id = [] {
  StaticString<20> peer_id = {{"-CK0001-"}};
  constexpr auto sz = sizeof("-CK0001-") / sizeof("-CK0001-"[0]);
  std::generate(peer_id.begin() + sz, peer_id.end(), [] {
    return details::utils::random<StaticString<20>::value_type>();
  });
#ifdef DEBUG
  spdlog::info("Peer ID: {}\nListen Port: {}", peer_id.data(), listen_port);
#endif
  return peer_id;
}();

inline const StaticString<19> pstr{'B', 'i', 't', 'T', 'o', 'r', 'r',
                                   'e', 'n', 't', ' ', 'p', 'r', 'o',
                                   't', 'o', 'c', 'o', 'l'};
// dunno how to delete \n in "str"
// In version 1.0 of the BitTorrent protocol,
// pstrlen = 19,
// and pstr = "BitTorrent protocol".
inline const char pstrlen = 19;  // length of pstr

}  // namespace opentorrent

#endif  // OPENTORRENT_CONSTANTS_HPP
