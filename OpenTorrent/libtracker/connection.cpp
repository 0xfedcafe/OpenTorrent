//
// Created by Linux Oid on 26.07.2020.
//
#include <details/utils/utility.hpp>
#include <liblogger/logger.hpp>
#include <libtracker/connection.hpp>
#include <libtracker/packets/udp/connect.hpp>

void opentorrent::udp::Connection::ConnHandle(
    const opentorrent::udp::Connection::ErrorCode& error_code,
    const opentorrent::udp::Connection::EndPoint& endpoint) {
  if (!socket_.is_open()) {
    Logger::get_instance()->Info("Connection request to " +
                                 endpoint.address().to_string() + " expired.");
    TryNext();
  } else if (error_code) {
    Logger::get_instance()->Error("Error in connection to " +
                                  endpoint.address().to_string());
    ErrorCode ec;
    socket_.close(ec);
    if (ec) {
      Logger::get_instance()->Error("Error in closing socket. " + ec.message());
      return;
    }
    TryNext();
  } else {
    Logger::get_instance()->Info("Socket is connected.");
    SendConnection();
  }
}

void opentorrent::udp::Connection::ConnIDExpireHandle(
    [[maybe_unused]] const opentorrent::udp::Connection::ErrorCode&
        error_code) {
  ErrorCode ec{};
  if (socket_.is_open()) socket_.cancel(ec);
  if (ec || error_code) {
    Logger::get_instance()->Error("Error in resending ConnectionPacket. " +
                                  ec.message());
  } else {
    Logger::get_instance()->Info("Connection ID expired.");
    socket_.cancel();
    connection_id_timer_.expires_at(DeadLineTimer::time_point::max());
    SendConnection();
  }
}

void opentorrent::udp::Connection::Connect() {
  if (iterator_ != end_points_.end()) {
    stopped_ = false;
    Logger::get_instance()->Info("Trying " + iterator_->host_name() + ":" +
                                 iterator_->service_name() + "...");
    boost::asio::async_connect(
        socket_, iterator_,
        [this](const ErrorCode& ec, const EndPoints::iterator& endpoint) {
          this->ConnHandle(ec, endpoint->endpoint());
        });
  }
}

void opentorrent::udp::Connection::TryNext() {
  Stop();
  time_out_ = TimeOut{15};
  if (iterator_ != end_points_.end()) {
    ++iterator_;
    Logger::get_instance()->Info("Next tracker.");
  }
  Connect();
}

void opentorrent::udp::Connection::ConnRequestTimeOut(
    const opentorrent::udp::Connection::ErrorCode& ec) {
  if (ec) {
    Logger::get_instance()->Error("Error in Connect request timeout. " +
                                  ec.message());
  } else {
    Logger::get_instance()->Info("Connect request timeout.");
    time_out_ *= 2;
    if (time_out_ >= biggest_timeout_) {
      TryNext();
    } else {
      SendConnection();
    }
  }
}

void opentorrent::udp::Connection::ConnPackHandle(
    int32_t transaction_id,
    const opentorrent::udp::Connection::ErrorCode& error,
    std::size_t bytes_transferred) {
  if (error || bytes_transferred < 16) {
    Logger::get_instance()->Error("Error in sending ConnectionPacket. " +
                                  std::to_string(bytes_transferred) +
                                  " bytes transferred.");
    SendConnection();
  } else {
    ReceiveConnection(transaction_id);
  }
}

void opentorrent::udp::Connection::SendConnection() {
  Logger::get_instance()->Info("Start sending connection request...");
  timer_.cancel();
  timer_.expires_after(time_out_);
  packets::udp::Connect send_packet{};
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

void opentorrent::udp::Connection::AnnRequestTimeOut(
    int64_t conn_id, const opentorrent::udp::Connection::ErrorCode& ec) {
  if (ec) {
    Logger::get_instance()->Error("Error in Announce request timeout. " +
                                  ec.message());
  } else {
    Logger::get_instance()->Info("Announce request timeout.");
    time_out_ *= 2;
    if (time_out_ >= biggest_timeout_) {
      TryNext();
    } else {
      SendAnnounce(conn_id);
    }
  }
}

void opentorrent::udp::Connection::ReceiveConnection(int32_t transaction_id) {
  socket_.async_receive(
      boost::asio::buffer(receive_conn_buf_),
      [this, transaction_id](const boost::system::error_code& error,
                             std::size_t bytes_transferred) {
        this->ReceiveConnHandle(transaction_id, error, bytes_transferred);
      });
}

void opentorrent::udp::Connection::ReceiveConnHandle(
    int32_t transaction_id,
    const opentorrent::udp::Connection::ErrorCode& error,
    std::size_t bytes_transferred) {
  if (error || bytes_transferred < 16) {
    Logger::get_instance()->Error("Error in receiving ConnectionPacket. " +
                                  std::to_string(bytes_transferred) +
                                  " bytes transferred.");
  } else {
    Logger::get_instance()->Info("Connection request received.");
    connection_id_timer_.expires_after(TimeOut{60});
    connection_id_timer_.async_wait(
        [this](const ErrorCode& ec) { this->ConnIDExpireHandle(ec); });
    packets::udp::ResponseConnect income{boost::asio::buffer(receive_conn_buf_),
                                         transaction_id};
    SendAnnounce(income.connectionID());
  }
}

void opentorrent::udp::Connection::ReceiveAnnHandle(
    int32_t transaction_id, const boost::system::error_code& error,
    std::size_t bytes_transferred) {
  if (error || bytes_transferred < 20) {
    Logger::get_instance()->Error("Error in sending Announce. " +
                                  std::to_string(bytes_transferred) +
                                  " bytes transferred.");
    SendAnnounce(transaction_id);
  } else {
    Logger::get_instance()->Info("Announce response received.");
    connection_id_timer_.expires_after(TimeOut{60});
    packets::udp::ResponseAnnounce income{boost::asio::buffer(receive_ann_buf_),
                                          transaction_id};
    if (!income.peers().empty()) {
      auto&& p = income.peers();
      std::copy(std::begin(p), std::end(p), std::back_inserter(peers_));
    }
    TryNext();
  }
}

void opentorrent::udp::Connection::ReceiveAnnounce(int32_t transaction_id) {
  Logger::get_instance()->Info("Receiving announce response.");
  socket_.async_receive(
      boost::asio::buffer(receive_ann_buf_),
      [this, transaction_id](const boost::system::error_code& error,
                             std::size_t bytes_transferred) {
        this->ReceiveAnnHandle(transaction_id, error, bytes_transferred);
      });
}

void opentorrent::udp::Connection::SendAnnHandler(
    int64_t conn_id, int32_t trans_id,
    const opentorrent::udp::Connection::ErrorCode& error,
    std::size_t bytes_transferred) {
  if (error) {
    Logger::get_instance()->Error("Error in sending Announce. " +
                                  std::to_string(bytes_transferred) +
                                  " bytes transferred.");
    SendAnnounce(conn_id);
  } else {
    Logger::get_instance()->Info("Announce request sent.");
    ReceiveAnnounce(trans_id);
  }
}

void opentorrent::udp::Connection::SendAnnounce(int64_t conn_id) {
  Logger::get_instance()->Info("Start sending announce...");
  timer_.cancel();
  timer_.expires_after(time_out_);
  std::uniform_int_distribution<uint32_t> distribution2;
  packets::udp::Announce send_announce(conn_id, info_hash_, 0, 0, 0, 2, 0, -1,
                                       1337, 0);
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

void opentorrent::udp::Connection::Stop() {
  socket_.close();
  timer_.cancel();
  connection_id_timer_.cancel();
  timer_.expires_at(DeadLineTimer::time_point::max());
  connection_id_timer_.expires_at(DeadLineTimer::time_point::max());
  stopped_ = true;
  Logger::get_instance()->Info("Tracker connection stopped.");
}

void opentorrent::udp::Connection::Run(
    const opentorrent::udp::Connection::EndPoints& end_points) {
  end_points_ = end_points;
  iterator_ = end_points_.begin();
  Connect();
}
