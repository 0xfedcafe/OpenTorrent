//
// Created by Linux Oid on 26.07.2020.
//

#ifndef COCKTORRENT_CONNECTION_H
#define COCKTORRENT_CONNECTION_H

#include <libtracker/packets/udp/announce.h>
#include <libtracker/packets/udp/connect.h>
#include <libtracker/packets/udp/response-announce.h>
#include <libtracker/packets/udp/response-connect.h>

#include <array>
#include <boost/asio.hpp>
#include <utility>
#include <vector>

namespace opentorrent::udp {
class Connection {
 public:
  using EndPoint = boost::asio::ip::udp::endpoint;
  using Socket = boost::asio::ip::udp::socket;
  using DeadLineTimer = boost::asio::steady_timer;
  using IOContext = boost::asio::io_context;
  using TimeOut = boost::asio::chrono::seconds;
  using EndPoints = boost::asio::ip::udp::resolver::results_type;
  using ErrorCode = boost::system::error_code;
  using ConnectBuffer = std::array<char, 16>;
  using InfoHashType = std::array<char, 20>;
  using Peers = std::vector<packets::udp::ResponseAnnounce::Seed>;

  Connection(IOContext &io_context, const InfoHashType &info_hash)
      : receive_ann_buf_(65535),
        socket_{io_context},
        timer_{io_context},
        connection_id_timer_{io_context},
        info_hash_{info_hash} {
    connection_id_timer_.expires_at(DeadLineTimer::time_point::max());
    timer_.expires_at(DeadLineTimer::time_point::max());
  }

  void Run(const EndPoints &end_points);

  void Stop();

  [[nodiscard]] bool stopped() const { return stopped_; }

  [[nodiscard]] Peers peers() const { return peers_; }

 private:
  void Connect();

  void SendAnnounce(int64_t conn_id);

  void SendAnnHandler(int64_t conn_id, int32_t trans_id, const ErrorCode &error,
                      std::size_t bytes_transferred);

  void ReceiveAnnounce(int32_t transaction_id);

  void ReceiveAnnHandle(int32_t transaction_id,
                        const boost::system::error_code &error,
                        std::size_t bytes_transferred);

  void SendConnection();

  void ReceiveConnection(int32_t transaction_id);

  void ReceiveConnHandle(int32_t transaction_id, const ErrorCode &error,
                         std::size_t bytes_transferred);

  void ConnPackHandle(int32_t transaction_id, const ErrorCode &error,
                      std::size_t bytes_transferred);

  void ConnHandle(const ErrorCode &error_code, const EndPoint &endpoint);

  void ConnIDExpireHandle(const ErrorCode &error_code);

  void ConnRequestTimeOut(const ErrorCode &ec);

  void AnnRequestTimeOut(int64_t conn_id, const ErrorCode &ec);

  void TryNext();

  static constexpr TimeOut biggest_timeout_ = TimeOut{3840};
  ConnectBuffer receive_conn_buf_{};
  std::vector<char> receive_ann_buf_;
  TimeOut time_out_{TimeOut{15}};
  packets::udp::Connect::BufferType send_conn_buf_{};
  packets::udp::Announce::BufferType send_ann_buf_{};
  Peers peers_{};
  bool stopped_{false};
  EndPoints::iterator iterator_;
  EndPoints end_points_;
  Socket socket_;
  DeadLineTimer timer_;
  DeadLineTimer connection_id_timer_;
  InfoHashType info_hash_;
};  // namespace opentorrent::udp
}  // namespace opentorrent::udp

#endif  // COCKTORRENT_CONNECTION_H
