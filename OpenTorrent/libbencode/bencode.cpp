#include "bencode.h"

#include <charconv>
#include <string>
#include <string_view>
#include <type_traits>
#include <variant>
#include <liblogger/logger.h>

#define PARSE_EXCEPTION_IF(CONDITION)                                 \
  do {                                                                \
    if ((CONDITION)) {                                                \
      Logger::get_instance()->Error("Bencode parsing error occured"); \
      throw std::invalid_argument("Bencode parsing error");           \
    }                                                                 \
  } while (0)

namespace opentorrent::bencode {

BencodeElement ParseBencodeElement(std::string_view &);

BencodeInt ParseInt(std::string_view &);

BencodeInt ParseBencodeInt(std::string_view &);

BencodeString ParseBencodeString(std::string_view &);

BencodeList ParseBencodeList(std::string_view &);

BencodeDictionary ParseBencodeDictionary(std::string_view &);

BencodeElement Decode(std::string_view expression) {
  auto result = ParseBencodeElement(expression);
  PARSE_EXCEPTION_IF(!expression.empty());
  return result;
}

BencodeElement ParseBencodeElement(std::string_view &expression) {
  BencodeElement result;
  char cur = expression.at(0);
  if (cur == 'i') {
    result.data = ParseBencodeInt(expression);
  } else if (cur == 'l') {
    result.data = ParseBencodeList(expression);
  } else if (cur == 'd') {
    result.data = ParseBencodeDictionary(expression);
  } else if (std::isdigit(cur)) {
    result.data = ParseBencodeString(expression);
  } else {
    PARSE_EXCEPTION_IF(true);
  }
  return result;
}

BencodeInt ParseInt(std::string_view &expression) {
  BencodeInt res;
  auto [p, ec] = std::from_chars(expression.data(),
                                 expression.data() + expression.size(), res);

  PARSE_EXCEPTION_IF(ec != std::errc());

  expression.remove_prefix(p - expression.data());
  return res;
}

BencodeInt ParseBencodeInt(std::string_view &expression) {
  PARSE_EXCEPTION_IF(expression.empty() || expression.at(0) != 'i');
  expression.remove_prefix(1);

  BencodeInt res = ParseInt(expression);
  PARSE_EXCEPTION_IF(expression.empty() || expression.at(0) != 'e');

  expression.remove_prefix(1);
  return res;
}

BencodeString ParseBencodeString(std::string_view &expression) {
  PARSE_EXCEPTION_IF(expression.empty() || !std::isdigit(expression.at(0)));

  BencodeInt stringLen = ParseInt(expression);

  PARSE_EXCEPTION_IF(expression.empty() || expression.at(0) != ':');

  expression.remove_prefix(1);

  PARSE_EXCEPTION_IF(expression.size() < static_cast<std::uintmax_t>(stringLen));

  BencodeString result = std::string(expression.substr(0, stringLen));
  expression.remove_prefix(stringLen);
  return result;
}

BencodeList ParseBencodeList(std::string_view &expression) {
  PARSE_EXCEPTION_IF(expression.empty() || expression.at(0) != 'l');
  expression.remove_prefix(1);
  BencodeList result;
  while (!expression.empty() && expression.at(0) != 'e') {
    result.push_back(ParseBencodeElement(expression));
  }
  expression.remove_prefix(1);
  return result;
}

BencodeDictionary ParseBencodeDictionary(std::string_view &expression) {
  PARSE_EXCEPTION_IF(expression.empty() || expression.at(0) != 'd');
  expression.remove_prefix(1);
  BencodeDictionary result;
  while (!expression.empty() && expression.at(0) != 'e') {
    BencodeString key = ParseBencodeString(expression);
    BencodeElement value = ParseBencodeElement(expression);
    result.insert({key, value});
  }
  expression.remove_prefix(1);
  return result;
}

std::string Encode(const BencodeElement &to_encode) {
  auto a = std::visit(
      [](auto &&arg) {
        if constexpr (std::is_same_v<std::decay_t<decltype(arg)>, BencodeInt>) {
          return "i" + std::to_string(arg) + "e";
        } else if constexpr (std::is_same_v<std::decay_t<decltype(arg)>,
                                            BencodeString>) {
          return std::to_string(arg.size()) + ":" + arg;
        } else if constexpr (std::is_same_v<std::decay_t<decltype(arg)>,
                                            BencodeList>) {
          std::string to_out{"l"};
          for (const auto &x : arg) {
            to_out += Encode(x);
          }
          to_out += "e";
          return to_out;
        } else {
          std::string to_out{"d"};
          for (const auto &x : arg) {
            to_out += Encode({x.first}) + Encode({x.second});
          }
          to_out += "e";
          return to_out;
        }
      },
      to_encode.data);
  return a;
}

}  // namespace cocktorrent::bencode
