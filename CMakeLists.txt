cmake_minimum_required(VERSION 3.12)

project(OpenTorrent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wpedantic -Werror -Wextra -fsanitize=address,undefined")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_DEBUG} -O2 -Wall -Wpedantic -Werror -Wextra")
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_COMPILER_LAUNCHER=ccache)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()
set(THREADS_PREFER_PTHREAD_FLAG ON)

include_directories(OpenTorrent)

set(SOURCES OpenTorrent/libbencode/bencode.cpp
        OpenTorrent/libbittorrent-protocol/torrent-file/basefile.cpp
        OpenTorrent/libbittorrent-protocol/torrent-file/multiplefile.cpp
        OpenTorrent/libbittorrent-protocol/torrent-file/singlefile.cpp
        OpenTorrent/libbittorrent-protocol/udp/tracker.hpp
        )
add_executable(${PROJECT_NAME} "OpenTorrent/open-torrent/main.cpp" ${SOURCES} OpenTorrent/libbittorrent-protocol/packets/udp/pack.hpp OpenTorrent/libbittorrent-protocol/constants.hpp)
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
        OpenTorrent/libbittorrent-protocol/view.hpp
        OpenTorrent/tests/scrape.cpp OpenTorrent/tests/announce.cpp
        OpenTorrent/libpeerwire-protocol/wire.cpp
        OpenTorrent/libpeerwire-protocol/wire.hpp
        OpenTorrent/libpeerwire-protocol/connection.cpp
        OpenTorrent/libpeerwire-protocol/connection.hpp
        OpenTorrent/libpeerwire-protocol/peer.cpp
        OpenTorrent/libpeerwire-protocol/peer.hpp
        )
add_executable(tests ${TEST_SOURCES} OpenTorrent/tests/main.cpp ${SOURCES})
target_compile_definitions(tests PUBLIC
        TEST_TORRENT_FILES_PATH=${PROJECT_SOURCE_DIR}/test-torrent-files
        #SPDLOG_FMT_EXTERNAL
        #SPDLOG_BUILD_SHARED
        )
target_link_libraries(tests ${CONAN_LIBS})
set_target_properties(tests
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests/bin")
