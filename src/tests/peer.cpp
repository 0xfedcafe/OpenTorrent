#include <torrentsinglefileinfo.h>
#include <udp/trackerconnection.h>
#include <boost/asio.hpp>
#include <ios>
#include <iostream>
#include <string>
#include <catch2/catch.hpp>
#include "bencode.h"
#include "logger.h"
#include "torrentmultiplefileinfo.h"
#include "utilities.h"

#define STRINGIFY2(X) #X
#define STRINGIFY(X) STRINGIFY2(X)

TEST_CASE("Get peers", "[torrent][tracker]") {
  using namespace cocktorrent;
  using namespace cocktorrent::udp;
  std::ifstream input_file(STRINGIFY(TEST_TORRENT_FILES_PATH) "/nsfw2.torrent",
                           std::ios::binary | std::ifstream::in);
  std::string expression{std::istreambuf_iterator<char>{input_file},
                         std::istreambuf_iterator<char>{}};
  auto res = bencode::Decode(expression);
  TorrentMultipleFileInfo s_file_s_info{res};
  input_file.close();
  boost::asio::io_service io_service;
  boost::asio::ip::udp::socket socket_{io_service};
  auto endpoints = util::GetUDPEndPoints(s_file_s_info.announce(), io_service);
  auto an_list_it = s_file_s_info.announce_list().begin();
  std::size_t peer_size{};
  while (an_list_it + 1 != s_file_s_info.announce_list().end()) {
    if (util::IsUdp(*an_list_it))
      endpoints = util::GetUDPEndPoints(*an_list_it, io_service);
    ++an_list_it;

    if (!endpoints.empty()) {
      TrackerConnection::DeadLineTimer deadline{io_service};
      deadline.expires_from_now(boost::asio::chrono::seconds(5));
      TrackerConnection tracker_connection{io_service,
                                           s_file_s_info.info_hash()};
      deadline.async_wait(
          [&]([[maybe_unused]] const TrackerConnection::ErrorCode &ec) {
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
