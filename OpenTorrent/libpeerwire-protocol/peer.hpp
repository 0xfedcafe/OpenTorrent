#ifndef OPENTORRENT_OPENTORRENT_LIBPEERWIRE_PROTOCOL_PEER_H_
#define OPENTORRENT_OPENTORRENT_LIBPEERWIRE_PROTOCOL_PEER_H_

#include "connection.hpp"
#include "libbittorrent-protocol/torrent-file/basefile.hpp"
namespace opentorrent {
class Peer {
  using State = Connection::kConnectionState;
  using Type = Connection::kMessageType;

 public:
  Peer(const BaseFile& file, int ip, int port);
  bool Choked();
  bool Interested();
  void Unchoke();
  int DownloadRate();
  int UploadRate();

 private:
  int ip_;
  int port_;
  int state_;
};
};  // namespace opentorrent
#endif
