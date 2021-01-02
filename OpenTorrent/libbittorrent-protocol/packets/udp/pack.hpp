//
// Created by vyacheslav on 1/1/21.
//

#ifndef OPENTORRENT_PACK_HPP
#define OPENTORRENT_PACK_HPP

#include <boost/asio/buffer.hpp>
#include <details/utils/net/utility.hpp>
#include <libbittorrent-protocol/big-endian.hpp>
#include <libbittorrent-protocol/constants.hpp>
#include <libbittorrent-protocol/view.hpp>
#include <string_view>
#include <type_traits>

namespace opentorrent::packets::udp {

enum class Packet { Connect = 0, Announce };
using ConstBuffer = boost::asio::const_buffer;

template <Packet>
struct PackRequest;

template <Packet>
struct UnpackResponse;

template <>
struct PackRequest<Packet::Connect> {
  struct PackRequestBuffer {
    BigEndian<int64_t> protocol_id{0x41727101980};
    BigEndian<int32_t> action{0};
    BigEndian<int32_t> transaction_id{details::utils::net::TransactionID()};

    [[nodiscard]] boost::asio::const_buffer buffer() const {
      return boost::asio::buffer(this, sizeof(*this));
    }
  };

  PackRequestBuffer operator()() { return PackRequestBuffer{}; }
};

template <>
struct UnpackResponse<Packet::Connect> {
  struct ConnectResponse {
    int32_t action;
    int32_t transaction_id;
    int64_t connection_id;

    [[nodiscard]] boost::asio::mutable_buffer buffer() {
      return boost::asio::buffer(this, sizeof(*this));
    }
  };

  ConnectResponse operator()(const ConstBuffer &buff) {
    const auto sv =
        std::string_view(static_cast<const char *>(buff.data()), buff.size());
    const auto [action, transaction_id, connection_id] = details::utils::Get<
        decltype(std::declval<ConnectResponse>().action),
        decltype(std::declval<ConnectResponse>().transaction_id),
        decltype(std::declval<ConnectResponse>().connection_id)>(sv);
    return ConnectResponse{action, transaction_id, connection_id};
  }
};

template <>
struct PackRequest<Packet::Announce> {
  struct PackRequestBuffer {
    template <class T>
    using Ignore = T;

    enum Event : int32_t { kNone = 0, kCompleted, kStarted, kStopped };

    BigEndian<int64_t> connection_id{};
    BigEndian<int32_t> action{1};
    BigEndian<int32_t> transaction_id{details::utils::net::TransactionID()};
    View<20> info_hash{};
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

  PackRequestBuffer operator()(int64_t connection_id, View<20> info_hash) {
    auto result = PackRequestBuffer{};
    result.connection_id = connection_id;
    result.info_hash = info_hash;
    return result;
  }
};

template <>
struct UnpackResponse<Packet::Announce> {
  struct AnnounceResponse {
    int32_t action;
    int32_t transaction_id;
    int32_t interval;
    int32_t leechers;
    int32_t seeders;
  };

  auto operator()(const ConstBuffer &buff) {
    const auto sv =
        std::string_view(static_cast<const char *>(buff.data()), buff.size());
    const auto [action, transaction_id, interval, leechers, seeders] =
        details::utils::Get<
            decltype(std::declval<AnnounceResponse>().action),
            decltype(std::declval<AnnounceResponse>().transaction_id),
            decltype(std::declval<AnnounceResponse>().interval),
            decltype(std::declval<AnnounceResponse>().leechers),
            decltype(std::declval<AnnounceResponse>().seeders)>(sv);
    return AnnounceResponse{action, transaction_id, interval, leechers,
                            seeders};
  }
};

}  // namespace opentorrent::packets::udp

#endif  // OPENTORRENT_PACK_HPP
