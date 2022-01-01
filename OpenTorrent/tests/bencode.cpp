#include <algorithm>
#include <catch2/catch.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <libbencode/bencode.hpp>

#define STRINGIFY2(X) #X
#define STRINGIFY(X) STRINGIFY2(X)

inline std::string ReadAll(const std::string& path) {
  std::ifstream input_file(path, std::ios::binary | std::ifstream::in);
  std::string expression{std::istreambuf_iterator<char>{input_file},
                         std::istreambuf_iterator<char>{}};
  return expression;
}

TEST_CASE("Bencode", "[torrent][bencode]") {
  // auto dir =
  //     std::filesystem::directory_iterator(STRINGIFY(TEST_TORRENT_FILES_PATH));
  // std::for_each(std::filesystem::begin(dir), std::filesystem::end(dir),
  //[](auto&& file) {
  //  auto bencode_str = ReadAll(file.path());
  //  auto decoded = opentorrent::bencode::Decode(bencode_str);
  //  auto encoded = opentorrent::bencode::Encode(decoded);
  //  auto decoded_twice = opentorrent::bencode::Decode(encoded);
  //              });
}
