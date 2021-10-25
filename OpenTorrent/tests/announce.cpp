#include <catch2/catch.hpp>
#include <fstream>
#include <libbencode/bencode.hpp>
#include <libbittorrent-protocol/torrent-file/multiplefile.hpp>
#include <libbittorrent-protocol/udp/tracker.hpp>

#define STRINGIFY2(X) #X
#define STRINGIFY(X) STRINGIFY2(X)

TEST_CASE("announce", "[torrent][tracker][udp]") {
  using namespace opentorrent;
  std::ifstream input_file(STRINGIFY(TEST_TORRENT_FILES_PATH) "/nsfw2.torrent",
                           std::ios::binary | std::ifstream::in);
  std::string expression{std::istreambuf_iterator<char>{input_file},
                         std::istreambuf_iterator<char>{}};
  auto res = bencode::Decode(expression);
  MultipleFile file_info{res};
  input_file.close();
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

  auto results = details::utils::net::GetUDPEndPoints(udp_announce, io_context);

  boost::asio::spawn(io_context, [&](const boost::asio::yield_context& yield) {
    tracker.AsyncConnect(results, yield);
    const auto peers = tracker.Announce(yield, file_info.info_hash());
    std::clog << peers.size() << '\n';
    for (const auto& p : peers) {
      std::clog << p.address() << ":" << p.port() << '\n';
    }
  });

  io_context.run();
}