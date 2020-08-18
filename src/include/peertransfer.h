//
// Created by Linux Oid on 27.07.2020.
//

#ifndef COCKTORRENT_PEERTRANSFER_H
#define COCKTORRENT_PEERTRANSFER_H

#include <logger.h>
#include <peer.h>
#include <peerhandshake.h>
#include <torrentbasefileinfo.h>
#include <udp/announcepacket.h>
#include <udp/responseannouncepacket.h>
#include <boost/asio.hpp>
#include <iterator>
#include <unordered_map>
#include <vector>
#include "utilities.h"

namespace cocktorrent::peer::tcp {
class PeerTransfer {
 public:
  using IOContext = boost::asio::io_context;
  using Ip = boost::asio::ip::tcp;
  using EndPoint = boost::asio::ip::tcp::endpoint;
  using Socket = boost::asio::ip::tcp::socket;
  using EndPoints = boost::asio::ip::tcp::resolver::results_type;
  using PeerSet = std::unordered_map<int, Peer>;
  using PeerArray = std::vector<Peer *>;
  using ErrorCode = boost::system::error_code;
  using Acceptor = boost::asio::ip::tcp::acceptor;
  template <class It>
  using EnableIfPeerIt = std::enable_if<std::is_same_v<
      std::decay_t<typename std::iterator_traits<It>::value_type>,
      udp::ResponseAnnouncePacket::Peer>>;

  PeerTransfer(IOContext &io_context, const TorrentBaseFileInfo &file_info);

  template <class PeerIt, typename = EnableIfPeerIt<PeerIt>>
  void Run(PeerIt b, PeerIt e) {
    acceptor_.async_accept(
        acceptor_sock_, [this](const boost::system::error_code &error) {
          if (error) {
            LOG_ERR("PeerTransfer: Error in accepting connection to " +
                     std::to_string(acceptor_endpoint_.port()) +
                    ". " + error.message());
          } else {
            LOG_INFO("PeerTransfer: Received connection from " +
                     acceptor_sock_.remote_endpoint().address().to_string() +
                     ":" +
                     std::to_string(acceptor_sock_.remote_endpoint().port()));
          }
        });
    active_peers_.reserve(std::distance(b, e));
    std::for_each(b, e, [this](auto &&el) {
      this->AsyncConnect(FromRespAnnPeer(std::forward<decltype(el)>(el)));
    });
  }

  [[nodiscard]] const PeerArray &active_peers() noexcept {
    return active_peers_;
  }

 private:
  void Start() {
    LOG_INFO("PeerTransfer: Starting peer selection policy algorithm...");
  }

  void UnchokeIntervalHandle() {
    switch (file_info_.local_state()) {
      case TorrentBaseFileInfo::LocalState::Leecher:
        std::sort(std::begin(active_peers_), std::end(active_peers_),
                  [](Peer *lhs, Peer *rhs) {
                    return lhs->upload_rate() > rhs->upload_rate();
                  });
        break;
      default:
        break;
    }
  }

  void AsyncConnect(const EndPoint &end_point);

  void AsyncConnectHandle(const ErrorCode &ec, int peer_id, Peer &peer);

  static EndPoint FromRespAnnPeer(udp::ResponseAnnouncePacket::Peer peer);

  TorrentBaseFileInfo file_info_;
  IOContext &io_context_;
  PeerSet peers_;
  PeerArray active_peers_;
  EndPoint acceptor_endpoint_{Ip::v4(), udp::AnnouncePacket::default_port};
  Acceptor acceptor_;
  Socket acceptor_sock_;
};
// namespace cocktorrent::peer::tcp
}  // namespace cocktorrent::peer::tcp

#endif  // COCKTORRENT_PEERTRANSFER_H
