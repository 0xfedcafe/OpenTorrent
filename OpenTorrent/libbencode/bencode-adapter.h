//
// Created by Linux Oid on 24.04.2020.
//

#ifndef COCKTORRENT_BENCODE_ADAPTER_H
#define COCKTORRENT_BENCODE_ADAPTER_H

#include <exception>
#include <variant>
#include <libbencode/bencode.h>

namespace opentorrent::bencode {
using BencodeCastError = std::bad_variant_access;
template <class T>
auto adapt(T *element);

template <class ElementType>
class BencodeElementAdapter {
 public:
  static_assert(
      std::is_same_v<bencode::BencodeElement, std::decay_t<ElementType>>,
      "ElementType of BencodeElementAdapter must be BencodeElement");
  using IntegerType = bencode::BencodeInt;
  using StringType = bencode::BencodeString;
  using ListType = bencode::BencodeList;
  using DictType = bencode::BencodeDictionary;
  using SizeType = bencode::BencodeList::size_type;

  BencodeElementAdapter(const BencodeElementAdapter &) = delete;
  BencodeElementAdapter &operator=(BencodeElementAdapter &) = delete;

  BencodeElementAdapter operator[](const StringType &s);

  BencodeElementAdapter operator[](const StringType &s) const;

  BencodeElementAdapter operator[](SizeType index);

  BencodeElementAdapter operator[](SizeType index) const;

  [[nodiscard]] bool Contains(const StringType &s) const;

  auto &integer();

  const auto &integer() const;

  auto &string();

  const auto &string() const;

  auto &list();

  const auto &list() const;

  auto &dictionary();

  const auto &dictionary() const;

  ElementType *element();

  ElementType *element() const;

 private:
  explicit BencodeElementAdapter(ElementType *element);
  friend auto adapt<ElementType>(ElementType *);

  ElementType *element_;
};

template <class T>
auto adapt(T *element) {
  return BencodeElementAdapter{element};
}

template <class ElementType>
BencodeElementAdapter<ElementType>::BencodeElementAdapter(ElementType *element)
    : element_(element) {}

template <class ElementType>
BencodeElementAdapter<ElementType>
BencodeElementAdapter<ElementType>::operator[](const StringType &s) {
  return BencodeElementAdapter(&dictionary().at(s));
}

template <class ElementType>
BencodeElementAdapter<ElementType>
BencodeElementAdapter<ElementType>::operator[](const StringType &s) const {
  return BencodeElementAdapter(&dictionary().at(s));
}

template <class ElementType>
BencodeElementAdapter<ElementType>
BencodeElementAdapter<ElementType>::operator[](SizeType index) const {
  return BencodeElementAdapter(&list().at(index));
}

template <class ElementType>
BencodeElementAdapter<ElementType>
BencodeElementAdapter<ElementType>::operator[](SizeType index) {
  return BencodeElementAdapter(&list().at(index));
}

template <class ElementType>
auto &BencodeElementAdapter<ElementType>::integer() {
  return std::get<IntegerType>(element_->data);
}

template <class ElementType>
const auto &BencodeElementAdapter<ElementType>::integer() const {
  return std::get<IntegerType>(element_->data);
}

template <class ElementType>
auto &BencodeElementAdapter<ElementType>::string() {
  return std::get<StringType>(element_->data);
}

template <class ElementType>
const auto &BencodeElementAdapter<ElementType>::string() const {
  return std::get<StringType>(element_->data);
}

template <class ElementType>
auto &BencodeElementAdapter<ElementType>::list() {
  return std::get<ListType>(element_->data);
}

template <class ElementType>
auto &BencodeElementAdapter<ElementType>::dictionary() {
  return std::get<DictType>(element_->data);
}

template <class ElementType>
const auto &BencodeElementAdapter<ElementType>::list() const {
  return std::get<ListType>(element_->data);
}

template <class ElementType>
const auto &BencodeElementAdapter<ElementType>::dictionary() const {
  return std::get<DictType>(element_->data);
}

template <class ElementType>
ElementType *BencodeElementAdapter<ElementType>::element() {
  return element_;
}

template <class ElementType>
ElementType *BencodeElementAdapter<ElementType>::element() const {
  return element_;
}

template <class ElementType>
bool BencodeElementAdapter<ElementType>::Contains(
    const BencodeElementAdapter::StringType &s) const {
  return dictionary().find(s) != dictionary().end();
}
}  // namespace opentorrent::bencode
#endif  // COCKTORRENT_BENCODE_ADAPTER_H
