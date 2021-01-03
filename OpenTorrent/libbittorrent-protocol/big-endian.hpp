//
// Created by vyacheslav on 1/2/21.
//

#ifndef OPENTORRENT_BIG_ENDIAN_HPP
#define OPENTORRENT_BIG_ENDIAN_HPP

#include <details/utils/utility.hpp>

namespace opentorrent {
template <class T>
struct BigEndian {
  static_assert(std::is_integral_v<T>);
  T value{};

  BigEndian() = default;
  explicit BigEndian(T val) : value(details::utils::HostToNetwork(val)) {}

  BigEndian &operator=(T val) {
    value = details::utils::HostToNetwork(val);
    return *this;
  }

  [[nodiscard]] bool operator==(BigEndian b) const { return value == b.value; }

  [[nodiscard]] T host() const { return details::utils::NetworkToHost(value); }
};

template <class T, typename = details::utils::EnableIfIntegral<T>>
[[nodiscard]] bool operator==(T lhs, BigEndian<T> rhs) {
  return lhs == rhs.host();
}

template <class T, typename = details::utils::EnableIfIntegral<T>>
[[nodiscard]] bool operator==(BigEndian<T> lhs, T rhs) {
  return lhs.host() == rhs;
}

}  // namespace opentorrent

#endif  // OPENTORRENT_BIG_ENDIAN_HPP
