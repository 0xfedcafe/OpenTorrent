#include <catch2/catch.hpp>
#include <details/utils/net/utility.hpp>
#include <filesystem>
#include <fstream>
#include <libbencode/bencode.hpp>
#include <libbittorrent-protocol/torrent-file/basefile.hpp>
#include <libbittorrent-protocol/torrent-file/multiplefile.hpp>
#include <libbittorrent-protocol/udp/tracker.hpp>

#define STRINGIFY2(X) #X
#define STRINGIFY(X) STRINGIFY2(X)

inline std::string ReadAll(const std::string& path) {
  std::ifstream input_file(path, std::ios::binary | std::ifstream::in);
  std::string expression{std::istreambuf_iterator<char>{input_file},
                         std::istreambuf_iterator<char>{}};
  return expression;
}

TEST_CASE("announce", "[torrent][tracker][udp]") {
  using namespace opentorrent;
  auto dir =
      std::filesystem::directory_iterator(STRINGIFY(TEST_TORRENT_FILES_PATH));
  //  INFO("Dir:" << dir->path());
  // REQUIRE(dir->path() != "\0");
  // std::ifstream input_file(
  //     "../../test-torrent-files/kali-linux-2021.4a-installer-amd64.iso.torrent",
  //     std::ios::binary | std::ifstream::in);
  // std::string expression{std::istreambuf_iterator<char>{input_file},
  //                        std::istreambuf_iterator<char>{}};

  std::for_each(
      std::filesystem::begin(dir), std::filesystem::end(dir), [&](auto&& file) {
        INFO("File Name::" << file.path());
        auto expression = ReadAll(file.path());
        auto res = bencode::Decode(expression);
        MultipleFile file_info{res};
        //   input_file.close();
        boost::asio::io_context io_context;
        udp::Tracker tracker(io_context);
        std::string_view udp_announce;
        for (auto&& announce : file_info.announce_list()) {
          if (details::utils::net::IsUdp(file_info.announce())) {
            udp_announce = file_info.announce();
            break;
          }
          if (details::utils::net::IsUdp(announce)) {
            udp_announce = announce;
            break;
          }
        }

        auto results =
            details::utils::net::GetUDPEndPoints(udp_announce, io_context);

        boost::asio::spawn(
            io_context, [&](const boost::asio::yield_context& yield) {
              tracker.AsyncConnect(results, yield);
              const auto peers = tracker.Announce(yield, file_info.info_hash());
              spdlog::info("Peers Size: {}\n", peers.size());
              for (const auto& p : peers) {
                spdlog::info("Address: {}:{}\n", p.address(), p.port());
              }
            });

        io_context.run();
      });
}
