//
// Created by Linux Oid on 24.04.2020.
//

#include <debug.h>
#include <spdlog/spdlog.h>
#include <libbittorrent-protocol/torrent-file/singlefile.hpp>
#include "debug.h"
namespace opentorrent {
SingleFile::SingleFile(const SingleFile::BencodeElement &el)
    : BaseFile(el), length_(adapt(&el)["info"]["length"].integer()) {
#if DEBUG == 1
  spdlog::info("Created File with Name: {}\nPieces:", name(), length());
#endif
}

SingleFile::Integer SingleFile::length() const { return length_; }
}  // namespace opentorrent
