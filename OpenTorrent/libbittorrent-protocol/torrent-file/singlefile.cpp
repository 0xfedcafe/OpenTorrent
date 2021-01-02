//
// Created by Linux Oid on 24.04.2020.
//

#include <libbittorrent-protocol/torrent-file/singlefile.hpp>

namespace opentorrent {
SingleFile::SingleFile(const SingleFile::BencodeElement &el)
    : BaseFile(el), length_(adapt(&el)["info"]["length"].integer()) {}

SingleFile::Integer SingleFile::length() const { return length_; }
}  // namespace opentorrent
