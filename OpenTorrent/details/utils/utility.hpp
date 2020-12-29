#ifndef UTILITIES_H
#define UTILITIES_H

#include <algorithm>
#include <boost/endian/conversion.hpp>
#include <charconv>
#include <chrono>
#include <cstring>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <liblogger/logger.hpp>
#include <random>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace details::utils {
template <class T>
using EnableIfIntegral =
    ::std::enable_if<::std::is_integral_v<::std::decay_t<T>>>;

[[nodiscard]] inline auto CurrentDate() {
  auto time = ::std::chrono::system_clock::now();
  auto tm = ::std::chrono::system_clock::to_time_t(time);
  return ::std::put_time(::std::localtime(&tm), "%Y-%m-%d %H:%M:%S");
}

namespace details {
inline ::std::mt19937 generator{::std::random_device{}()};
}

template <class T, typename = EnableIfIntegral<T>>
[[nodiscard]] T random() {
  std::uniform_int_distribution<T> distribution;
  return distribution(details::generator);
}

template <class T, class InputIt>
[[nodiscard]] ::std::vector<T> ToVector(InputIt b, InputIt e) {
  return ::std::vector<T>{b, e};
}

template <class T, class Cont>
[[nodiscard]] ::std::vector<T> ToVector(Cont &&c) {
  auto b = ::std::begin(::std::forward<Cont>(c));
  auto e = ::std::begin(::std::forward<Cont>(c));
  return ::details::utils::ToVector<T>(b, e);
}

template <size_t Size>
struct CharSequence {
  char chars[Size];
};

template <class T, typename = EnableIfIntegral<T>>
[[nodiscard]] T HostToNetwork(T x) noexcept {
  return ::boost::endian::native_to_big(x);
}

template <class T, typename = EnableIfIntegral<T>>
[[nodiscard]] T NetworkToHost(T x) noexcept {
  return ::boost::endian::big_to_native(x);
}

template <class T, typename = EnableIfIntegral<T>>
[[nodiscard]] ::details::utils::CharSequence<sizeof(T)> ToNetworkCharSequence(T x) {
  ::details::utils::CharSequence<sizeof(T)> chars;
  x = ::details::utils::HostToNetwork(x);
  ::std::memcpy(chars.chars, &x, sizeof(T));
  return chars;
}

template <class T, typename = EnableIfIntegral<T>>
[[nodiscard]] T FromNetworkCharSequence(::std::string_view bytes) {
  using namespace std::literals;
  T value;
  if (bytes.size() != sizeof(T))
    throw ::std::logic_error("sizeof(T) {" + std::to_string(sizeof(T)) +
                             "} must be equals to bytes.size() {" +
                             std::to_string(bytes.size()) +
                             "} in "
                             "FromNetworkCharSequence(std::string_view).");
  ::std::memcpy(&value, bytes.data(), sizeof(T));
  return ::details::utils::NetworkToHost(value);
}

namespace detail {

inline void Put(char *buf, ::std::string_view sv) {
  ::std::memcpy(buf, sv.data(), sv.size());
}

template <class T, typename = EnableIfIntegral<T>>
void Put(char *buf, T el) {
  ::std::memcpy(buf, ::details::utils::ToNetworkCharSequence(el).chars,
                sizeof(el));
}

template <size_t N>
void Put(char *buf, ::std::array<char, N> ar) {
  ::std::memcpy(buf, ar.data(), sizeof(ar));
}

template <class... T>
void Put(char *buf, T &&...els) {
  [[maybe_unused]] int dummy_arr[sizeof...(T)] = {
      (::details::utils::detail::Put(buf, ::std::forward<T>(els)),
       buf += sizeof(T), 0)...};
}
}  // namespace detail

template <class... T, size_t N>
void Put([[maybe_unused]] ::std::array<char, N> &buf,
         [[maybe_unused]] T &&...els) {
  constexpr size_t all_size = (sizeof(T) + ...);
  static_assert(N == all_size, "Array size mismatch.");
  ::details::utils::detail::Put(buf.data(), std::forward<T>(els)...);
}

}  // namespace details::utils

#endif  // UTILITIES_H
