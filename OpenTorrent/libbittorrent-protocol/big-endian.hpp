//
// Created by vyacheslav on 1/2/21.
//

#ifndef OPENTORRENT_BIG_ENDIAN_HPP
#define OPENTORRENT_BIG_ENDIAN_HPP

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

  T host() { return details::utils::NetworkToHost(value); }
};
}  // namespace opentorrent

#endif  // OPENTORRENT_BIG_ENDIAN_HPP
