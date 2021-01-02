//
// Created by vyacheslav on 1/2/21.
//

#ifndef OPENTORRENT_VIEW_HPP
#define OPENTORRENT_VIEW_HPP

namespace opentorrent {
template <size_t N>
struct View {
  const int8_t *start_{};

  [[nodiscard]] constexpr const int8_t *start() const { return start_; }
  [[nodiscard]] constexpr auto size() const { return N; }
};

template <size_t N>
using StaticString = std::array<int8_t, N>;
}  // namespace opentorrent

#endif  // OPENTORRENT_VIEW_HPP
