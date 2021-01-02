//
// Created by vyacheslav on 1/1/21.
//

#ifndef OPENTORRENT_TRACKER_PEERS_HPP
#define OPENTORRENT_TRACKER_PEERS_HPP

#include <boost/asio/ip/udp.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/coroutine2/coroutine.hpp>
#include <chrono>

#include <libbittorrent-protocol/packets/udp/pack.hpp>

namespace opentorrent::udp {

using Peer = boost::asio::ip::udp::endpoint;
using Coroutine = boost::coroutines2::coroutine<Peer>;
using ResolverResults = boost::asio::ip::udp::resolver::results_type;
using Socket = boost::asio::ip::udp::socket;
using YieldContext = boost::asio::yield_context;
using Timer = boost::asio::deadline_timer;
using Seconds = boost::posix_time::seconds;
using ConnectionId = int64_t;

using namespace opentorrent::packets::udp;

namespace peers::details {

inline ConnectionId Connection(Socket &s, const YieldContext &yield) {
  assert(s.is_open());

  auto timer = Timer{s.get_executor()};
  auto expire_time = Seconds{15};
  auto connection_success = false;

  while (!connection_success && expire_time < Seconds{3840 /* 15 * 2^8 */}) {
    timer.expires_from_now(expire_time);
    expire_time *= 2;
    timer.async_wait([&](const boost::system::error_code &error) {
      if (error) {
        Logger::instance()->Error(std::string{"Error in timeout's waiting "} +
                                  error.message());
        return;
      }
      s.cancel();
    });

    boost::system::error_code ec;
    const auto req_buff = PackRequest<Packet::Connect>{}();
    s.async_send(req_buff.buffer(), yield[ec]);

    if (ec) {
      Logger::instance()->Error("Error in sending connection request" +
                                ec.message());
      continue;
    }

    UnpackResponse<Packet::Connect>::ConnectResponse resp{};
    s.async_receive(resp.buffer(), yield[ec]);

    if (ec) {
      Logger::instance()->Error("Error in receiving connection response" +
                                ec.message());
      continue;
    }

    resp = UnpackResponse<Packet::Connect>{}(resp.buffer());

    if (resp.action == 0 /* connection */
        && resp.transaction_id == req_buff.transaction_id) {
      return resp.connection_id;
    }
  }

  throw std::invalid_argument{"Can't connect to " +
                              s.remote_endpoint().address().to_string()};
}

inline void Announce(Socket &s,
                     ConnectionId connection_id,
                     Coroutine::push_type &push,
                     YieldContext yield) {}
}  // namespace peers::details

template <class ExecutionContext>
void GetPeers(ExecutionContext &context, Coroutine::push_type &push,
              const ResolverResults &eps, YieldContext yield) {
  auto soc = Socket{context};
  boost::asio::async_connect(soc, eps, yield);
  peers::details::Connection(soc, yield);
  //  peers::details::Announce(soc, push, yield);
}

}  // namespace opentorrent::udp

#endif  // OPENTORRENT_TRACKER_PEERS_HPP
