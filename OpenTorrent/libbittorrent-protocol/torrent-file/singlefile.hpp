//
// Created by Linux Oid on 24.04.2020.
//

#ifndef OPENTORRENT_SINGLEFILE_H
#define OPENTORRENT_SINGLEFILE_H

#include <libbencode/bencode-adapter.hpp>
#include <libbencode/bencode.hpp>
#include <libbittorrent-protocol/torrent-file/basefile.hpp>

namespace opentorrent {
class SingleFile : public BaseFile {
 public:
  using Dictionary = BaseFile::Dictionary;
  using String = BaseFile::String;
  using Integer = BaseFile::Integer;
  using BencodeElement = BaseFile::BencodeElement;
  using BencodeAdapter = BaseFile::BencodeAdapter;

  SingleFile() = delete;

  explicit SingleFile(BencodeElement const &el);

  [[nodiscard]] Integer length() const;

 private:
  Integer length_{};
};
}  // namespace opentorrent

#endif  // OPENTORRENT_SINGLEFILE_H
