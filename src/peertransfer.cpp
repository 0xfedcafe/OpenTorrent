//
// Created by Linux Oid on 27.07.2020.
//

#include <peertransfer.h>
#include <utilities.h>

#include <logger.h>
#include <string>
#include <utility>

cocktorrent::peer::tcp::PeerTransfer::PeerTransfer(
    PeerTransfer::IOContext &io_context, const TorrentBaseFileInfo &file_info)
    : file_info_{file_info},
      io_context_{io_context},
      peers_{},
      acceptor_{io_context, acceptor_endpoint_},
      acceptor_sock_{io_context} {}

cocktorrent::peer::tcp::PeerTransfer::EndPoint
cocktorrent::peer::tcp::PeerTransfer::FromRespAnnPeer(
    udp::ResponseAnnouncePacket::Peer peer) {
  auto char_seq = util::FromIPv4(peer.ip);
  std::string ip_string;
  ip_string.reserve(15);
  for (auto ch : char_seq.chars) {
    ip_string += std::to_string(static_cast<int>(ch));
    ip_string += '.';
  }
  ip_string.erase(ip_string.cend() - 1);
  auto ip_address = boost::asio::ip::make_address(ip_string);
  return EndPoint{ip_address, peer.port};
}

void cocktorrent::peer::tcp::PeerTransfer::AsyncConnectHandle(
    const cocktorrent::peer::tcp::PeerTransfer::ErrorCode &ec, int peer_id,
    cocktorrent::peer::tcp::Peer &peer) {
  if (ec) {
    LOG_ERR("PeerTransfer: Error in connecting. " + ec.message() +
            " peer_id: " + std::to_string(peer_id));
    peers_.erase(peer_id);
  } else {
    LOG_INFO("PeerTransfer: Peer connected to " + peer.address().to_string());
    peer.HandShake(
        [this, &peer] {
          this->active_peers_.push_back(&peer);
          if (this->active_peers_.size() == 3) {
            this->Start();
          }
        },
        file_info_.info_hash());
  }
}

void cocktorrent::peer::tcp::PeerTransfer::AsyncConnect(
    const cocktorrent::peer::tcp::PeerTransfer::EndPoint &end_point) {
  LOG_INFO("PeerTransfer: Start connecting peer to " +
           end_point.address().to_string() + ":" +
           std::to_string(end_point.port()));
  std::uniform_int_distribution<int> distribution{};
  auto [it, f] = peers_.emplace(
      std::make_pair(distribution(util::generator), Peer{io_context_}));
  if (f) {
    auto it_c = it;
    LOG_INFO("PeerTransfer: Peer inserted. " + std::to_string(it_c->first));
    it->second.socket().async_connect(
        end_point, [this, it_c](const ErrorCode &ec) {
          this->AsyncConnectHandle(ec, it_c->first, it_c->second);
        });
  } else {
    LOG_ERR("PeerTransfer: Peer not inserted.");
  }
}
