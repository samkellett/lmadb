#ifndef LMADB_CXX_ADT_CONST_STR_HPP
#define LMADB_CXX_ADT_CONST_STR_HPP

#include <fmt/format.h>

#include <array>
#include <string_view>

namespace lmadb::cxx {

template <std::size_t N>
class const_str {
public:
  template <size_t M>
  const_str(const char(&str)[M]);
  const_str(std::string_view str);

  auto string() const -> std::string;

  auto operator==(const const_str &other) const -> bool;
  auto operator<(const const_str &other) const -> bool;

private:
  std::array<char, N> data_;
};

template <size_t N>
template <size_t M>
const_str<N>::const_str(const char(&str)[M])
: data_{}
{
  static_assert(M <= N, "C string too large to fit in const_str.");
  std::copy(std::begin(str), std::begin(str) + M, std::begin(data_));
}

template <std::size_t N>
const_str<N>::const_str(const std::string_view str)
: data_{}
{
  if (str.size() > N) {
    throw std::runtime_error{fmt::format("String '{}' too large to fit.", str)};
  }

  std::copy(std::begin(str), std::begin(str) + std::min(str.size(), N), std::begin(data_));
}

template <std::size_t N>
auto const_str<N>::string() const -> std::string
{
  return std::string{data_.data(), N};
}

template <std::size_t N>
auto const_str<N>::operator==(const const_str<N> &other) const -> bool
{
  return data_ == other.data_;
}

template <std::size_t N>
auto const_str<N>::operator<(const const_str<N> &other) const -> bool
{
  return std::string_view{data_.data(), N} < std::string_view{other.data_.data(), N};
}

} // namespace lmadb::cxx

#endif // LMADB_CXX_ADT_CONST_STR_HPP
