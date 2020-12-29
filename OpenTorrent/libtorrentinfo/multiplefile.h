//
// Created by Linux Oid on 28.04.2020.
//

#ifndef COCKTORRENT_MULTIPLEFILE_H
#define COCKTORRENT_MULTIPLEFILE_H

#include <libbencode/bencode-adapter.h>
#include <libbencode/bencode.h>
#include <libtorrentinfo/basefile.h>
#include <vector>

namespace opentorrent {
/**
 * @class MultipleFile
 * @brief Класс для хранения инфы в случае нескольких файлов
 * @author https://github.com/s3ponia
 */
class MultipleFile : public BaseFile {
 public:
  struct File;
  using InfoDictionary = BaseFile::Dictionary;
  using String = BaseFile::String;
  using Integer = BaseFile::Integer;
  using List = std::vector<File>;
  using BencodeElement = BaseFile::BencodeElement;
  using BencodeAdapter = BaseFile::BencodeAdapter;

  struct File {
    using List = std::vector<String>;

    explicit File(BencodeElement const &dict);

    static List ListFromBencode(const BencodeAdapter::ListType &el);

    Integer length;
    List path;
  };

  explicit MultipleFile(BencodeElement const &el);

  [[nodiscard]] const List &files() const;

 private:
  List files_;
};
}  // namespace opentorrent

#endif  // COCKTORRENT_MULTIPLEFILE_H
