#include <catch2/catch.hpp>
#include <fstream>
#include <libbencode/bencode.hpp>
#include <libbittorrent-protocol/torrent-file/multiplefile.hpp>
#include <libbittorrent-protocol/udp/tracker.hpp>

#define STRINGIFY2(X) #X
#define STRINGIFY(X) STRINGIFY2(X)

TEST_CASE("scrape", "[torrent][tracker][udp]") {
  using namespace opentorrent;
  std::ifstream input_file(
      "../../test-torrent-files/kali-linux-2021.4a-installer-amd64.iso.torrent",
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
    auto scrape_info = tracker.Scrape(yield, file_info.info_hash());
    spdlog::info("Completed: {}\nLeechers: {}\nSeeders: {}\n",
                 scrape_info.completed, scrape_info.leechers,
                 scrape_info.seeders);
  });

  io_context.run();
}
