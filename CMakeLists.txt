cmake_minimum_required(VERSION 3.12)

project(CockTorrent)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wpedantic -Werror -Wextra -fsanitize=address,undefined")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_DEBUG} -O2 -Wall -Wpedantic -Werror -Wextra")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()
set(THREADS_PREFER_PTHREAD_FLAG ON)

include_directories(OpenTorrent)

set(SOURCES OpenTorrent/libbencode/bencode.cpp
            OpenTorrent/liblogger/logger.cpp
            OpenTorrent/libpeer/peerhandshake.cpp
        OpenTorrent/libtorrentinfo/basefile.cpp
        OpenTorrent/libtorrentinfo/multiplefile.cpp
        OpenTorrent/libtorrentinfo/singlefile.cpp
            OpenTorrent/libtracker/connection.cpp
        OpenTorrent/libtracker/packets/udp/announce.cpp
        OpenTorrent/libtracker/packets/udp/connect.cpp
        OpenTorrent/libtracker/packets/udp/response-announce.cpp
        OpenTorrent/libtracker/packets/udp/response-connect.cpp)
add_executable(${PROJECT_NAME} "OpenTorrent/open-torrent/main.cpp" ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${CONAN_LIBS})

set_target_properties(${PROJECT_NAME}
  PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")


# Unit testing
file(GLOB TEST_SOURCES "OpenTorrent/tests/*.cpp")
set(TEST_SOURCES OpenTorrent/tests/bencode.cpp
                 OpenTorrent/tests/getudpendpoints.cpp
                 OpenTorrent/tests/peer.cpp)
add_executable(tests ${TEST_SOURCES} OpenTorrent/tests/main.cpp ${SOURCES})
target_compile_definitions(tests PUBLIC
  TEST_TORRENT_FILES_PATH=${PROJECT_SOURCE_DIR}/test-torrent-files)
target_link_libraries(tests ${CONAN_LIBS})
set_target_properties(tests
  PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests/lib"
  LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests/lib"
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests/bin")
