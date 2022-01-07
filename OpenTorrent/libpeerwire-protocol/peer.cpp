#include "peer.hpp"
#include <cstdint>
#include <utility>
#include "debug.h"
#include "details/utils/utility.hpp"
#include "libbittorrent-protocol/constants.hpp"
#include "libbittorrent-protocol/torrent-file/basefile.hpp"
#include "libbittorrent-protocol/view.hpp"
namespace opentorrent {
Peer::Peer(const BaseFile& file, int ip, int port) : ip_(ip), port_(port) {
  state_ = State::kImChoking | State::kPeerChoking;
  std::array<char, (49 + pstrlen)> handshake =
      details::utils::PackInStdArray<decltype(pstrlen), decltype(pstr), int64_t,
                                     decltype(file.info_hash()),
                                     decltype(peer_id)>(
          std::move(pstrlen), std::move(pstr), int64_t{0},
          std::move(file.info_hash()), std::move(peer_id));
#ifdef DEBUG
  spdlog::info("Sending Handshake {}\n", handshake.data());
#endif
}
};  // namespace opentorrent
