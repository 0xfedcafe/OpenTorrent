#ifndef BENCODE_H
#define BENCODE_H

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

[[nodiscard]] constexpr bool operator==(const BencodeElement &lhs,
                          const BencodeElement &rhs) {
  return lhs.data == rhs.data;
}

[[nodiscard]] BencodeElement Decode(std::string_view);
[[nodiscard]] std::string Encode(const BencodeElement &);
}  // namespace opentorrent::bencode

#endif  // BENCODE_H
