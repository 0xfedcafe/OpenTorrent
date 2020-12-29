#include "bencode.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <catch2/catch.hpp>

#define STRINGIFY2(X) #X
#define STRINGIFY(X) STRINGIFY2(X)

inline std::string ReadAll(const std::string& path) {
  std::ifstream input_file(path, std::ios::binary | std::ifstream::in);
  std::string expression{std::istreambuf_iterator<char>{input_file},
                         std::istreambuf_iterator<char>{}};
  return expression;
}

TEST_CASE("Bencode", "[torrent][bencode]") {
  auto dir =
      std::filesystem::directory_iterator(STRINGIFY(TEST_TORRENT_FILES_PATH));
  std::for_each(std::filesystem::begin(dir), std::filesystem::end(dir),
                [](auto&& file) {
                  auto bencode_str = ReadAll(file.path());
                  auto decoded = cocktorrent::bencode::Decode(bencode_str);
                  auto encoded = cocktorrent::bencode::Encode(decoded);
                  auto decoded_twice = cocktorrent::bencode::Decode(encoded);
                  REQUIRE(decoded == decoded_twice);
                  REQUIRE(bencode_str == encoded);
                });
}
