//
// Created by vyacheslav on 1/1/21.
//

#ifndef OPENTORRENT_TRACKER_HPP
#define OPENTORRENT_TRACKER_HPP

#include <spdlog/spdlog.h>
#include <boost/asio/ip/udp.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/coroutine2/coroutine.hpp>
#include <chrono>
#include <libbittorrent-protocol/packets/udp/pack.hpp>
#include <optional>
#include "debug.h"

namespace opentorrent::udp {

using Peer = boost::asio::ip::udp::endpoint;
using ResolverResults = boost::asio::ip::udp::resolver::results_type;
using Socket = boost::asio::ip::udp::socket;
using YieldContext = boost::asio::yield_context;
using Timer = boost::asio::deadline_timer;
using TimePoint = std::chrono::time_point<std::chrono::system_clock>;
using Seconds = boost::posix_time::seconds;
using ConnectionId = int64_t;

using namespace opentorrent::packets::udp;

class Tracker : boost::noncopyable {
  Socket sock;
  TimePoint last_connection;
  ConnectionId connection_id{};

  void Connection(const YieldContext &yield) {
    auto timer = Timer{sock.get_executor()};
    auto expire_time = Seconds{15};

    while (expire_time < Seconds{3840 /* 15 * 2^8 */}) {
      timer.expires_from_now(expire_time);
      expire_time *= 2;
      timer.async_wait([&](const boost::system::error_code &error) {
        if (error) {
          return;
        }
        sock.cancel();
      });

      const auto resp =
          Request<Packet::Connect>(PackRequest<Packet::Connect>{}(), yield);
      if (resp.has_value()) {
        connection_id = resp.value().connection_id;
        last_connection = std::chrono::system_clock::now();
        return;
      }
    }

    throw std::invalid_argument{"Can't connect to " +
                                sock.remote_endpoint().address().to_string()};
  }

  void AutoConnect(const YieldContext &yield) {
    if (!IsConnected()) {
      Connection(yield);
    }
  }

  [[nodiscard]] bool IsConnected() const {
    using namespace std::chrono_literals;
    return std::chrono::system_clock::now() - last_connection < 60s;
  }

  template <Packet packet, class ExecutionContext>
  [[nodiscard]] std::optional<typename UnpackResponse<packet>::Response>
  Request(typename PackRequest<packet>::Request pack_request,
          ExecutionContext &&yield) {
    boost::system::error_code ec;
    sock.async_send(pack_request.buffer(), yield[ec]);

    if (ec) {
      return std::nullopt;
    }

    typename UnpackResponse<packet>::Response resp{};
    sock.async_receive(resp.buffer(), yield[ec]);

    if (ec) {
      return std::nullopt;
    }

    resp = UnpackResponse<packet>{}(resp.buffer());

    if (resp.action == pack_request.action &&
        resp.transaction_id == pack_request.transaction_id) {
      return std::optional{resp};
    }

    return std::nullopt;
  }

 public:
  template <class ExecutionContext>
  explicit Tracker(ExecutionContext &context) : sock(context) {}

  template <class Handler>
  void AsyncConnect(const ResolverResults &eps, Handler &&yield) {
    boost::asio::async_connect(sock, eps, yield);
  }

  template <class Handler>
  std::vector<Peer> Announce(Handler &&yield, StaticString<20> info_hash) {
    auto timer = Timer{sock.get_executor()};
    auto expire_time = Seconds{15};
    std::vector<char> package{};
    boost::system::error_code ec;

    while (expire_time < Seconds{3840 /* 15 * 2^8 */}) {
      timer.cancel();

      AutoConnect(yield);

      timer.expires_from_now(expire_time);
      expire_time *= 2;
      timer.async_wait([&](const boost::system::error_code &error) {
        if (error) {
          spdlog::info("ERROR: {}", error.to_string());
          return;
        }
        sock.cancel();
      });

      const auto req =
          PackRequest<Packet::Announce>{}(connection_id, info_hash);
      sock.async_send(req.buffer(), yield[ec]);

      if (ec) {
        continue;
      }

      auto null_buff = boost::asio::null_buffers();
      sock.async_receive(null_buff, decltype(sock)::message_peek, yield[ec]);

      if (ec) {
        continue;
      }

      package.resize(sock.available());
      sock.async_receive(boost::asio::buffer(package), yield[ec]);

      if (ec) {
        continue;
      }

      const auto resp =
          UnpackResponse<Packet::Announce>{}(boost::asio::buffer(package));

      if (resp.action == req.action &&
          resp.transaction_id == req.transaction_id) {
        const auto peers =
            UnpackResponse<Packet::Announce>::UnpackPeers(boost::asio::buffer(
                package.data() + sizeof(resp), package.size() - sizeof(resp)));
        std::vector<Peer> result;
        std::cout << peers.size();
        result.reserve(peers.size());
        for (auto &p : peers) {
          boost::asio::ip::address_v4 address(
              details::utils::ToNetworkCharSequence(p.ip));
          result.emplace_back(address, p.port);

#ifdef DEBUG
          spdlog::info("New Peer with IP: {} and Port: {}", address, p.port);
#endif
        }
        return result;
      }
    }
    spdlog::info("Peer announce failed with exception:");
    throw std::invalid_argument{"Can't scrape from " +
                                sock.remote_endpoint().address().to_string()};
  }

  template <class Handler>
  UnpackResponse<Packet::Scrape>::Response Scrape(Handler &&yield,
                                                  StaticString<20> info_hash) {
    auto timer = Timer{sock.get_executor()};
    auto expire_time = Seconds{15};

    while (expire_time < Seconds{3840 /* 15 * 2^8 */}) {
      timer.cancel();

      AutoConnect(yield);

      timer.expires_from_now(expire_time);
      expire_time *= 2;
      timer.async_wait([&](const boost::system::error_code &error) {
        if (error) {
          return;
        }
        sock.cancel();
      });

      const auto resp = Request<Packet::Scrape>(
          PackRequest<Packet::Scrape>{}(connection_id, info_hash), yield);

      if (resp.has_value()) {
        return resp.value();
      }
    }

    throw std::invalid_argument{"Can't scrape from " +
                                sock.remote_endpoint().address().to_string()};
  }
};

}  // namespace opentorrent::udp

#endif  // OPENTORRENT_TRACKER_HPP
