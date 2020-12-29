//
// Created by Linux Oid on 28.04.2020.
//

#include <libtorrentinfo/multiplefile.h>
namespace opentorrent {
MultipleFile::MultipleFile(
    const MultipleFile::BencodeElement &el)
    : BaseFile(el),
      files_{adapt(&el)["info"]["files"].list().cbegin(),
             adapt(&el)["info"]["files"].list().cend()} {}

const MultipleFile::List &MultipleFile::files() const {
  return files_;
}

MultipleFile::File::File(
    const MultipleFile::BencodeElement &dict)
    : length(adapt(&dict)["length"].integer()),
      path(ListFromBencode(adapt(&dict)["path"].list())) {}

MultipleFile::File::List MultipleFile::File::ListFromBencode(
    const BencodeAdapter::ListType &el) {
  class LasyIterator {
   public:
    using difference_type = ptrdiff_t;
    using value_type = const String;
    using pointer = value_type *;
    using reference = value_type &;
    using iterator_category = std::input_iterator_tag;

    explicit LasyIterator(
        const BencodeAdapter::ListType::const_iterator &iterator)
        : iterator_(iterator) {}

    LasyIterator &operator++() {
      ++iterator_;
      return *this;
    }

    LasyIterator operator++(int) {
      auto save = *this;
      ++iterator_;
      return save;
    }

    reference operator*() const { return adapt(&*iterator_).string(); }

    bool operator==(const LasyIterator &rhs) const {
      return iterator_ == rhs.iterator_;
    }

    bool operator!=(const LasyIterator &rhs) const { return !(rhs == *this); }

   private:
    BencodeAdapter::ListType::const_iterator iterator_;
  };
  return MultipleFile::File::List{LasyIterator{el.cbegin()},
                                             LasyIterator{el.cend()}};
}
}  // namespace cocktorrent
