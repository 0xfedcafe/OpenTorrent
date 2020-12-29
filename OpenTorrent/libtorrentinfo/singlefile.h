//
// Created by Linux Oid on 24.04.2020.
//

#ifndef COCKTORRENT_SINGLEFILE_H
#define COCKTORRENT_SINGLEFILE_H

#include <libbencode/bencode-adapter.h>
#include <libbencode/bencode.h>
#include <libtorrentinfo/basefile.h>

namespace opentorrent {
/**
 * @class SingleFile
 * @brief Класс для хранения инфы в случае одного файлов
 * @author https://github.com/s3ponia
 */
class SingleFile : public BaseFile {
 public:
  using Dictionary = BaseFile::Dictionary;
  using String = BaseFile::String;
  using Integer = BaseFile::Integer;
  using BencodeElement = BaseFile::BencodeElement;
  using BencodeAdapter = BaseFile::BencodeAdapter;

  SingleFile() = delete;

  explicit SingleFile(BencodeElement const &el);

  Integer length() const;

 private:
  Integer length_{};
};
}  // namespace opentorrent

#endif  // COCKTORRENT_SINGLEFILE_H
