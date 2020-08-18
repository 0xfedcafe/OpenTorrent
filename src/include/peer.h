#ifndef COCKTORRENT_PEER_H
#define COCKTORRENT_PEER_H

#include <logger.h>
#include <peerhandshake.h>
#include <boost/asio.hpp>

#include <utility>

namespace cocktorrent::peer::tcp {
class Peer {
 public:
  using IOContext = boost::asio::io_context;
  using Socket = boost::asio::ip::tcp::socket;
  using ErrorCode = boost::system::error_code;
  using HandShakeBuffer = PeerHandshake::BufferType;
  using InfoHashType = std::array<char, 20>;

  enum PeerState : unsigned { INTERESTED = 0, CHOCKED = 2 };

  using UnderlyingType =
      std::make_unsigned_t<std::underlying_type_t<PeerState>>;

  explicit Peer(IOContext &io_context) : socket_(io_context) {}

  void SetChoked() noexcept { state_ |= PeerState::CHOCKED; }

  void SetInterested() noexcept { state_ |= PeerState::INTERESTED; }

  void UnsetChoked() noexcept { state_ &= ~PeerState::CHOCKED; }

  void UnsetInterested() noexcept { state_ &= ~PeerState::INTERESTED; }

  void upload(intmax_t uploaded) noexcept { upload_rate_ += uploaded; }

  template <class F>
  void HandShake(F &&callback, const InfoHashType &info_hash) {
    info_hash_ = info_hash;
    if (socket_.is_open()) SendHandshake(std::forward<F>(callback));
  }

  [[nodiscard]] bool Interested() const noexcept {
    return state_ & PeerState::INTERESTED;
  }

  [[nodiscard]] bool Choked() const noexcept {
    return state_ & PeerState::CHOCKED;
  }

  [[nodiscard]] bool Unchoked() const noexcept { return !Choked(); }

  [[nodiscard]] bool NotInterested() const noexcept { return !Interested(); }

  [[nodiscard]] const auto &socket() const noexcept { return socket_; }

  [[nodiscard]] auto &socket() noexcept { return socket_; }

  [[nodiscard]] auto address() const noexcept {
    return socket_.remote_endpoint().address();
  }

  [[nodiscard]] auto port() const noexcept {
    return socket_.remote_endpoint().port();
  }

  [[nodiscard]] intmax_t upload_rate() const noexcept { return upload_rate_; }

 private:
  template <class F>
  void SendHandshake(F &&f) {
    LOG_INFO("Peer: Start sending handshake to " + address().to_string() +
             "...");
    PeerHandshake send{info_hash_};
    buff_ = send.buffer();
    boost::asio::async_write(
        socket_, boost::asio::buffer(buff_),
        [this, callback = std::forward<F>(f)](
            const ErrorCode &error, std::size_t bytes_transferred) mutable {
          this->SendHandshakeHandle(callback, error, bytes_transferred);
        });
  }

  template <class F>
  void SendHandshakeHandle(F &&callback, const ErrorCode &error,
                           std::size_t bytes_transferred) {
    if (error) {
      LOG_ERR("Peer: Error in sending Handshake to " + address().to_string() +
              " " + std::to_string(bytes_transferred) + " bytes sended.");
    } else {
      LOG_INFO("Peer: Handshake sended to " + address().to_string() + ". " +
               std::to_string(bytes_transferred) + " bytes sended.");
      ReceiveHandShake(std::forward<F>(callback));
    }
  }

  template <class F>
  void ReceiveHandShake(F &&callback) {
    socket_.async_receive(
        boost::asio::buffer(buff_),
        [this, callback = std::forward<F>(callback)](
            const ErrorCode &error, std::size_t bytes_transferred) {
          this->ReceiveHandShakeHandle(callback, error, bytes_transferred);
        });
  }

  template <class F>
  void ReceiveHandShakeHandle(F &&callback, const ErrorCode &error,
                              std::size_t bytes_transferred) {
    if (error) {
      LOG_ERR("Peer: Error in receiving Handshake. " +
              std::to_string(bytes_transferred) + " bytes received. " +
              error.message());
    } else {
      PeerHandshake received{buff_};
      if (received.info_hash() != info_hash_) {
        LOG_ERR("Peer: Error in receiving Handshake to " +
                address().to_string() + " info hash mismatch.");
        socket_.close();
      } else {
        LOG_INFO("Peer: Handshake received from " + address().to_string());
        callback();
      }
    }
  }

  friend std::hash<cocktorrent::peer::tcp::Peer>;

  InfoHashType info_hash_{};
  HandShakeBuffer buff_{};
  Socket socket_;
  intmax_t upload_rate_{};
  unsigned state_{PeerState::INTERESTED | PeerState::CHOCKED};
};
}  // namespace cocktorrent::peer::tcp

#endif
