//
// Created by prise on 7/2/20.
//

#include <details/utils/utility.hpp>
#include <libpeer/peerhandshake.hpp>

namespace opentorrent::peer::tcp {
PeerHandshake::PeerHandshake(const std::array<char, 20> &info_hash)
    : info_hash_(info_hash) {
  std::generate(peer_id_.begin() + 8,
                peer_id_.end(), []() { return 0; });
}
}  // namespace opentorrent::peer::tcp
