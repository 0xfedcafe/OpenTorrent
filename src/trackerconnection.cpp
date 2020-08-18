//
// Created by Linux Oid on 26.07.2020.
//
#include <logger.h>
#include <udp/trackerconnection.h>
#include <string>
#define CURR_ADDRESS                                              \
  (iterator_ != end_points_.end()                                 \
       ? iterator_->host_name() + ":" + iterator_->service_name() \
       : "")

void cocktorrent::udp::TrackerConnection::ConnHandle(
    const cocktorrent::udp::TrackerConnection::ErrorCode& error_code) {
  if (!socket_.is_open()) {
    LOG_INFO("TrackerConnection: Connection request to " + CURR_ADDRESS +
             " expired.");
    TryNext();
  } else if (error_code) {
    LOG_ERR("TrackerConnection: Error in connection to " + CURR_ADDRESS);
    ErrorCode ec;
    socket_.close(ec);
    if (ec) {
      LOG_ERR("TrackerConnection: Error in closing socket. " + ec.message());
      return;
    }
    TryNext();
  } else {
    LOG_INFO("TrackerConnection: Socket is connected to " +
             socket_.remote_endpoint().address().to_string() + ".");
    SendConnection();
  }
}

void cocktorrent::udp::TrackerConnection::ConnIDExpireHandle([
    [maybe_unused]] const cocktorrent::udp::TrackerConnection::ErrorCode&
                                                                 error_code) {
  ErrorCode ec{};
  if (socket_.is_open()) socket_.cancel(ec);
  if (ec || error_code) {
    LOG_ERR("TrackerConnection: Error in resending ConnectionPacket to " +
            CURR_ADDRESS + ". " + (ec ? ec : error_code).message());
  } else {
    LOG_INFO("TrackerConnection: Connection ID expired. " + CURR_ADDRESS);
    socket_.cancel();
    connection_id_timer_.expires_at(DeadLineTimer::time_point::max());
    SendConnection();
  }
}

void cocktorrent::udp::TrackerConnection::Connect() {
  if (iterator_ != end_points_.end()) {
    stopped_ = false;
    LOG_INFO("Trying " + CURR_ADDRESS + "...");
    boost::asio::async_connect(
        socket_, iterator_,
        [this](const ErrorCode& ec,
               [[maybe_unused]] const EndPoints::iterator& endpoint) {
          this->ConnHandle(ec);
        });
  }
}

void cocktorrent::udp::TrackerConnection::TryNext() {
  Stop();
  time_out_ = TimeOut{15};
  if (iterator_ != end_points_.end()) {
    ++iterator_;
  }
  if (iterator_ != end_points_.end()) {
    LOG_INFO("TrackerConnection: Next tracker.");
    Connect();
  }
}

void cocktorrent::udp::TrackerConnection::ConnRequestTimeOut(
    const cocktorrent::udp::TrackerConnection::ErrorCode& ec) {
  if (ec) {
    LOG_ERR("TrackerConnection: Error in Connect request to " + CURR_ADDRESS +
            " timeout. " + ec.message());
  } else {
    LOG_INFO("TrackerConnection: Connect request to " + CURR_ADDRESS +
             " timeout.");
    time_out_ *= 2;
    if (time_out_ >= biggest_timeout_) {
      TryNext();
    } else {
      socket_.cancel();
      SendConnection();
    }
  }
}

void cocktorrent::udp::TrackerConnection::ConnPackHandle(
    int32_t transaction_id,
    const cocktorrent::udp::TrackerConnection::ErrorCode& error,
    std::size_t bytes_transferred) {
  if (error || bytes_transferred < 16) {
    LOG_ERR("TrackerConnection: Error in sending ConnectionPacket to " +
            CURR_ADDRESS + ". " + std::to_string(bytes_transferred) +
            " bytes transferred. " + error.message());
    SendConnection();
  } else {
    ReceiveConnection(transaction_id);
  }
}

void cocktorrent::udp::TrackerConnection::SendConnection() {
  LOG_INFO("TrackerConnection: Start sending connection request to " +
           CURR_ADDRESS + "...");
  timer_.cancel();
  timer_.expires_after(time_out_);
  ConnectPacket send_packet{};
  send_conn_buf_ = send_packet.buffer();
  int32_t transaction_id = send_packet.transactionID();
  socket_.async_send(boost::asio::buffer(send_conn_buf_),
                     [this, transaction_id](const ErrorCode& error,
                                            std::size_t bytes_transferred) {
                       this->ConnPackHandle(transaction_id, error,
                                            bytes_transferred);
                     });
  timer_.async_wait(
      [this](const ErrorCode& ec) { this->ConnRequestTimeOut(ec); });
}

void cocktorrent::udp::TrackerConnection::AnnRequestTimeOut(
    int64_t conn_id, const cocktorrent::udp::TrackerConnection::ErrorCode& ec) {
  if (ec) {
    LOG_ERR("TrackerConnection: Error in Announce request to " + CURR_ADDRESS +
            " timeout. " + ec.message());
  } else {
    LOG_INFO("TrackerConnection: Announce request to " + CURR_ADDRESS +
             " timeout.");
    time_out_ *= 2;
    if (time_out_ >= biggest_timeout_) {
      TryNext();
    } else {
      SendAnnounce(conn_id);
    }
  }
}

void cocktorrent::udp::TrackerConnection::ReceiveConnection(
    int32_t transaction_id) {
  socket_.async_receive(
      boost::asio::buffer(receive_conn_buf_),
      [this, transaction_id](const boost::system::error_code& error,
                             std::size_t bytes_transferred) {
        this->ReceiveConnHandle(transaction_id, error, bytes_transferred);
      });
}

void cocktorrent::udp::TrackerConnection::ReceiveConnHandle(
    int32_t transaction_id,
    const cocktorrent::udp::TrackerConnection::ErrorCode& error,
    std::size_t bytes_transferred) {
  if (error || bytes_transferred < 16) {
    LOG_ERR("TrackerConnection: Error in receiving ConnectionPacket from " +
            CURR_ADDRESS + ". " + std::to_string(bytes_transferred) +
            " bytes transferred. " + error.message());
  } else {
    LOG_INFO("TrackerConnection: Connection request received from " +
             CURR_ADDRESS + ".");
    connection_id_timer_.expires_after(TimeOut{60});
    connection_id_timer_.async_wait(
        [this](const ErrorCode& ec) { this->ConnIDExpireHandle(ec); });
    ResponseConnectPacket income{boost::asio::buffer(receive_conn_buf_),
                                 transaction_id};
    SendAnnounce(income.connectionID());
  }
}

void cocktorrent::udp::TrackerConnection::ReceiveAnnHandle(
    int32_t transaction_id, const boost::system::error_code& error,
    std::size_t bytes_transferred) {
  if (error || bytes_transferred < 20) {
    LOG_ERR("TrackerConnection: Error in sending AnnouncePacket to " +
            CURR_ADDRESS + ". " + std::to_string(bytes_transferred) +
            " bytes transferred.");
    SendAnnounce(transaction_id);
  } else {
    connection_id_timer_.expires_after(TimeOut{60});
    ResponseAnnouncePacket income{boost::asio::buffer(receive_ann_buf_),
                                  transaction_id};
    if (!income.peers().empty()) {
      auto&& p = income.peers();
      std::copy(std::begin(p), std::end(p), std::back_inserter(peers_));
      LOG_INFO("TrackerConnection: Announce response received from " +
               CURR_ADDRESS + ". " + std::to_string(p.size()) +
               " peers received.");
    }
    Stop();
  }
}

void cocktorrent::udp::TrackerConnection::ReceiveAnnounce(
    int32_t transaction_id) {
  LOG_INFO("TrackerConnection: Receiving announce response from " +
           CURR_ADDRESS + ".");
  socket_.async_receive(
      boost::asio::buffer(receive_ann_buf_),
      [this, transaction_id](const boost::system::error_code& error,
                             std::size_t bytes_transferred) {
        this->ReceiveAnnHandle(transaction_id, error, bytes_transferred);
      });
}

void cocktorrent::udp::TrackerConnection::SendAnnHandler(
    int64_t conn_id, int32_t trans_id,
    const cocktorrent::udp::TrackerConnection::ErrorCode& error,
    std::size_t bytes_transferred) {
  if (error) {
    LOG_ERR("TrackerConnection: Error in sending AnnouncePacket to " +
            CURR_ADDRESS + ". " + std::to_string(bytes_transferred) +
            " bytes transferred.");
    SendAnnounce(conn_id);
  } else {
    LOG_INFO("TrackerConnection: Announce request sent to " + CURR_ADDRESS +
             ".");
    ReceiveAnnounce(trans_id);
  }
}

void cocktorrent::udp::TrackerConnection::SendAnnounce(int64_t conn_id) {
  LOG_INFO("TrackerConnection: Start sending announce to " + CURR_ADDRESS +
           "...");
  timer_.cancel();
  timer_.expires_after(time_out_);
  std::uniform_int_distribution<char> distribution;
  AnnouncePacket send_announce(conn_id, file_info_,
                               AnnouncePacket::default_port);
  send_ann_buf_ = send_announce.buffer();
  int32_t trans_id = send_announce.transactionID();
  socket_.async_send(boost::asio::buffer(send_ann_buf_),
                     [this, conn_id, trans_id](const ErrorCode& ec,
                                               std::size_t bytes_transferred) {
                       this->SendAnnHandler(conn_id, trans_id, ec,
                                            bytes_transferred);
                     });
  timer_.async_wait([this, conn_id](const ErrorCode& ec) {
    this->AnnRequestTimeOut(conn_id, ec);
  });
}

void cocktorrent::udp::TrackerConnection::Stop() {
  socket_.close();
  timer_.cancel();
  connection_id_timer_.cancel();
  timer_.expires_at(DeadLineTimer::time_point::max());
  connection_id_timer_.expires_at(DeadLineTimer::time_point::max());
  stopped_ = true;
  LOG_INFO("Tracker connection stopped.");
}

void cocktorrent::udp::TrackerConnection::Run(
    const cocktorrent::udp::TrackerConnection::EndPoints& end_points) {
  end_points_ = end_points;
  iterator_ = end_points_.begin();
  Connect();
}
