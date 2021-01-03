//
// Created by Linux Oid on 28.04.2020.
//

#ifndef OPENTORRENT_BASEFILE_H
#define OPENTORRENT_BASEFILE_H

#include <libbencode/bencode-adapter.hpp>
#include <vector>

namespace opentorrent {
class BaseFile {
 public:
  using Dictionary = bencode::BencodeDictionary;
  using String = bencode::BencodeString;
  using Integer = bencode::BencodeInt;
  using BencodeElement = bencode::BencodeElement;
  using BencodeAdapter = bencode::BencodeElementAdapter<const BencodeElement>;
  using AnnounceList = std::vector<std::string>;
  using InfoHashType = std::array<int8_t, 20>;

  BaseFile() = delete;

  explicit BaseFile(BencodeElement const &el);

  [[nodiscard]] const String &announce() const { return announce_; }

  [[nodiscard]] const AnnounceList &announce_list() const {
    return announce_list_;
  }

  [[nodiscard]] const String &pieces() const { return pieces_; }

  [[nodiscard]] const String &name() const { return name_; }

  [[nodiscard]] const InfoHashType &info_hash() const { return info_hash_; }

  [[nodiscard]] Integer piece_length() const { return piece_length_; }

 private:
  String announce_{};
  AnnounceList announce_list_{};
  String pieces_{};
  String name_{};
  InfoHashType info_hash_{};
  Integer piece_length_{};
};
}  // namespace opentorrent

#endif  // OPENTORRENT_BASEFILE_H
