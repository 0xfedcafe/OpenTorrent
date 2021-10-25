//
// Created by prise on 10/24/21.
//

#ifndef OPENTORRENT_OPENTORRENT_LIBPEERWIRE_PROTOCOL_CONNECTION_H_
#define OPENTORRENT_OPENTORRENT_LIBPEERWIRE_PROTOCOL_CONNECTION_H_


class Connection {

  enum class kMessageType {
    kChoke = 0,
    kUnChoke = 1,
    kInterested = 1,
    kNotInterested = 3,
    kHave = 4,
    kBitField = 5,
    kRequest = 6,
    kPiece = 7,
    kCancel = 8,
    kPort = 9
  };
  enum class kConnectionState {
    kHandshakeSent = (1UL << 0),
    kHandshakeReceived = (1UL << 1),
    kDisconnected = (1UL << 2),
    kBitfieldReceived = (1UL << 3),
    kConnected = (1UL << 4),
    kUnreachablePeer = (1UL << 5),
    kImChoking = (1UL << 6),
    kImInterested = (1UL << 7),
    kPeerChoking = (1UL << 8),
    kPeerInterested = (1UL << 9),
    kConnectionFailed = (1UL << 10)
  };


};

#endif  // OPENTORRENT_OPENTORRENT_LIBPEERWIRE_PROTOCOL_CONNECTION_H_
