//
// Created by vyacheslav on 1/1/21.
//

#ifndef OPENTORRENT_PACK_HPP
#define OPENTORRENT_PACK_HPP
#define DEBUG_MODE
#include <boost/asio/buffer.hpp>
#include <details/utils/net/utility.hpp>
#include <libbittorrent-protocol/big-endian.hpp>
#include <libbittorrent-protocol/constants.hpp>
#include <libbittorrent-protocol/view.hpp>
#include <string_view>
#include <type_traits>
namespace opentorrent::packets::udp {

enum class Packet { Connect = 0, Announce, Scrape };
using ConstBuffer = boost::asio::const_buffer;

template <Packet>
struct PackRequest;

template <Packet>
struct UnpackResponse;

template <>
struct PackRequest<Packet::Connect> {
  struct Request {
    BigEndian<int64_t> protocol_id{0x41727101980};
    BigEndian<int32_t> action{0};
    BigEndian<int32_t> transaction_id{details::utils::net::TransactionID()};

    [[nodiscard]] boost::asio::const_buffer buffer() const {
      return boost::asio::buffer(this, sizeof(*this));
    }
  };

  Request operator()() { return Request{}; }
};

template <>
struct UnpackResponse<Packet::Connect> {
  struct Response {
    int32_t action;
    int32_t transaction_id;
    int64_t connection_id;

    [[nodiscard]] boost::asio::mutable_buffer buffer() {
      return boost::asio::buffer(this, sizeof(*this));
    }
  };

  Response operator()(const ConstBuffer &buff) {
    const auto sv =
        std::string_view(static_cast<const char *>(buff.data()), buff.size());
    const auto tuple =
        details::utils::Get<decltype(std::declval<Response>().action),
                            decltype(std::declval<Response>().transaction_id),
                            decltype(std::declval<Response>().connection_id)>(
            sv);
    const auto [action, transaction_id, connection_id] = tuple;

    return Response{action, transaction_id, connection_id};
  }
};

template <>
struct PackRequest<Packet::Announce> {
  struct Request {
    template <class T>
    using Ignore = T;

    enum Event : int32_t { kNone = 0, kCompleted, kStarted, kStopped };

    BigEndian<int64_t> connection_id{};
    BigEndian<int32_t> action{1};
    BigEndian<int32_t> transaction_id{details::utils::net::TransactionID()};
    StaticString<20> info_hash{};
    StaticString<20> peer_id{opentorrent::peer_id};
    BigEndian<int64_t> downloaded{};
    BigEndian<int64_t> left{};
    BigEndian<int64_t> uploaded{};
    BigEndian<int32_t> event{Event::kStarted};
    Ignore<BigEndian<uint32_t>> ip_address{};
    BigEndian<uint32_t> key{details::utils::random<uint32_t>()};
    BigEndian<int32_t> num_want{-1};
    BigEndian<uint16_t> port{opentorrent::listen_port};

    [[nodiscard]] boost::asio::const_buffer buffer() const {
      return boost::asio::buffer(this, sizeof(*this));
    }
  };

  Request operator()(int64_t connection_id, StaticString<20> info_hash) {
    auto result = Request{};
    result.connection_id = connection_id;
    result.info_hash = info_hash;
    return result;
  }
};

template <>
struct UnpackResponse<Packet::Announce> {
  struct Peer {
    int32_t ip;
    uint16_t port;
  };

  struct Response {
    int32_t action;
    int32_t transaction_id;
    int32_t interval;
    int32_t leechers;
    int32_t seeders;

    [[nodiscard]] boost::asio::mutable_buffer buffer() {
      return boost::asio::buffer(this, sizeof(*this));
    }
  };

  auto operator()(const ConstBuffer &buff) {
    const auto sv =
        std::string_view(static_cast<const char *>(buff.data()), buff.size());
    const auto [action, transaction_id, interval, leechers, seeders] =
        details::utils::Get<decltype(std::declval<Response>().action),
                            decltype(std::declval<Response>().transaction_id),
                            decltype(std::declval<Response>().interval),
                            decltype(std::declval<Response>().leechers),
                            decltype(std::declval<Response>().seeders)>(sv);
    return Response{action, transaction_id, interval, leechers, seeders};
  }

  [[nodiscard]] static std::vector<Peer> UnpackPeers(const ConstBuffer &buff) {
    std::vector<Peer> peers;
    auto sv_peers =
        std::string_view(static_cast<const char *>(buff.data()), buff.size());

    peers.resize(sv_peers.size() / (sizeof(int32_t) + sizeof(uint16_t)));

    for (auto &el : peers) {
      std::tie(el.ip, el.port) =
          details::utils::Get<int32_t, uint16_t>(sv_peers);
      sv_peers.remove_prefix(sizeof(int32_t) + sizeof(uint16_t));
    }

    return peers;
  }
};

template <>
struct PackRequest<Packet::Scrape> {
  struct Request {
    BigEndian<int64_t> connection_id;
    BigEndian<int32_t> action{2};
    BigEndian<int32_t> transaction_id{details::utils::net::TransactionID()};
    StaticString<20> info_hash;

    [[nodiscard]] boost::asio::const_buffer buffer() const {
      return boost::asio::buffer(this, sizeof(*this));
    }
  };

  Request operator()(int64_t connection_id, StaticString<20> info_hash) {
    Request result{};
    result.connection_id = connection_id;
    result.info_hash = info_hash;
    return result;
  }
};

template <>
struct UnpackResponse<Packet::Scrape> {
  struct Response {
    int32_t action;
    int32_t transaction_id;
    int32_t seeders;
    int32_t completed;
    int32_t leechers;

    [[nodiscard]] boost::asio::mutable_buffer buffer() {
      return boost::asio::buffer(this, sizeof(*this));
    }
  };

  auto operator()(const ConstBuffer &buff) {
    Response result{};
    const auto sv =
        std::string_view(static_cast<const char *>(buff.data()), buff.size());
    std::tie(result.action, result.transaction_id, result.seeders,
             result.completed, result.leechers) =
        details::utils::Get<decltype(std::declval<Response>().action),
                            decltype(std::declval<Response>().transaction_id),
                            decltype(std::declval<Response>().seeders),
                            decltype(std::declval<Response>().completed),
                            decltype(std::declval<Response>().leechers)>(sv);

    return result;
  }
};

}  // namespace opentorrent::packets::udp

#endif  // OPENTORRENT_PACK_HPP
