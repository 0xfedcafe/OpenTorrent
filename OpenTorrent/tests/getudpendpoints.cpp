#include <algorithm>
#include <catch2/catch.hpp>
#include <details/utils/net/utility.hpp>
#include <filesystem>
#include <fstream>
#include <libbencode/bencode.hpp>
#include <libbittorrent-protocol/torrent-file/basefile.hpp>

#define STRINGIFY2(X) #X
#define STRINGIFY(X) STRINGIFY2(X)

inline std::string ReadAll(const std::string& path) {
  std::ifstream input_file(path, std::ios::binary | std::ifstream::in);
  std::string expression{std::istreambuf_iterator<char>{input_file},
                         std::istreambuf_iterator<char>{}};
  return expression;
}

TEST_CASE("UDP", "[torrent][udp][resolver]") {
  auto dir =
      std::filesystem::directory_iterator(STRINGIFY(TEST_TORRENT_FILES_PATH));
  boost::asio::io_service io_service;
  std::for_each(
      std::filesystem::begin(dir), std::filesystem::end(dir), [&](auto&& file) {
        auto bencode_str = ReadAll(file.path());
        auto decoded = opentorrent::bencode::Decode(bencode_str);
        opentorrent::BaseFile file_info{decoded};
        if (file_info.announce().find("udp://") == 0) {
          auto vec = details::utils::net::GetUDPEndPoints(file_info.announce(),
                                                          io_service);
          INFO(file_info.announce());
          REQUIRE(!vec.empty());
        }
        auto&& ann_list = file_info.announce_list();
        std::for_each(ann_list.begin(), ann_list.end(), [&](auto&& el) {
          if (std::forward<decltype(el)>(el).find("udp://") == 0) {
            auto vec = details::utils::net::GetUDPEndPoints(
                std::forward<decltype(el)>(el), io_service);
            // INFO(std::forward<decltype(el)>(el));
            REQUIRE(!vec.empty());
          }
        });
      });
}
