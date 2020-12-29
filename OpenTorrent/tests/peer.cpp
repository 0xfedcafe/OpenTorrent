#include <boost/asio.hpp>
#include <catch2/catch.hpp>
#include <details/utils/net/utility.hpp>
#include <libbencode/bencode.hpp>
#include <liblogger/logger.hpp>
#include <libtorrentinfo/multiplefile.hpp>
#include <libtracker/connection.hpp>
#include <string>

#define STRINGIFY2(X) #X
#define STRINGIFY(X) STRINGIFY2(X)

TEST_CASE("Get peers", "[torrent][tracker]") {
  using namespace opentorrent;
  using namespace opentorrent::udp;
  std::ifstream input_file(STRINGIFY(TEST_TORRENT_FILES_PATH) "/nsfw2.torrent",
                           std::ios::binary | std::ifstream::in);
  std::string expression{std::istreambuf_iterator<char>{input_file},
                         std::istreambuf_iterator<char>{}};
  auto res = bencode::Decode(expression);
  MultipleFile s_file_s_info{res};
  input_file.close();
  boost::asio::io_service io_service;
  boost::asio::ip::udp::socket socket_{io_service};
  auto endpoints = details::utils::net::GetUDPEndPoints(
      s_file_s_info.announce(), io_service);
  auto an_list_it = s_file_s_info.announce_list().begin();
  std::size_t peer_size{};
  while (an_list_it + 1 != s_file_s_info.announce_list().end()) {
    if (details::utils::net::IsUdp(*an_list_it))
      endpoints = details::utils::net::GetUDPEndPoints(*an_list_it, io_service);
    ++an_list_it;

    if (!endpoints.empty()) {
      Connection::DeadLineTimer deadline{io_service};
      deadline.expires_from_now(boost::asio::chrono::seconds(5));
      Connection tracker_connection{io_service, s_file_s_info.info_hash()};
      deadline.async_wait(
          [&]([[maybe_unused]] const Connection::ErrorCode &ec) {
            Logger::get_instance()->Error(ec.message());
            io_service.stop();
            peer_size += tracker_connection.peers().size();
          });
      tracker_connection.Run(endpoints);

      io_service.run();
    }
  }

  REQUIRE(peer_size > 0);
}
