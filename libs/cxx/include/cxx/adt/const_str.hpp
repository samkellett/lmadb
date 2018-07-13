#ifndef LMADB_CXX_ADT_CONST_STR_HPP
#define LMADB_CXX_ADT_CONST_STR_HPP

#include <fmt/format.h>

#include <array>
#include <string_view>

namespace lmadb::cxx {

template <std::size_t N>
class const_str {
public:
  const_str(std::string_view str);

  auto operator==(const const_str &other) const;
  auto operator<(const const_str &other) const;

private:
  std::array<char, N> data_;
};

template <std::size_t N>
const_str<N>::const_str(const std::string_view str)
: data_{}
{
  if (str.size() > N) {
    throw std::runtime_error{fmt::format("String '{}' too large to fit.")};
  }

  std::copy(std::begin(str), std::begin(str) + std::min(str.size(), N), std::begin(data_));
}

template <std::size_t N>
auto const_str<N>::operator==(const const_str<N> &other) const
{
  return data_ == other.data_;
}

template <std::size_t N>
auto const_str<N>::operator<(const const_str<N> &other) const
{
  return std::string_view{data_.data(), N} < std::string_view{other.data_.data(), N};
}

} // namespace lmadb::cxx

#endif // LMADB_CXX_ADT_CONST_STR_HPP
