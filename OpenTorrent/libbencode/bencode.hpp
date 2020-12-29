#ifndef BENCODE_H
#define BENCODE_H

#include <liblogger/logger.hpp>
#include <map>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace opentorrent::bencode {

struct BencodeElement;
using BencodeInt = long long;
using BencodeString = std::string;
using BencodeDictionary = std::map<BencodeString, BencodeElement>;
using BencodeList = std::vector<BencodeElement>;

struct BencodeElement {
  std::variant<BencodeInt, BencodeString, BencodeDictionary, BencodeList> data;
};

constexpr bool operator==(const BencodeElement &lhs,
                          const BencodeElement &rhs) {
  return lhs.data == rhs.data;
}

BencodeElement Decode(std::string_view);
std::string Encode(const BencodeElement &);
}  // namespace opentorrent::bencode

#endif  // BENCODE_H
