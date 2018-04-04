#ifndef LMADB_CXX_TYPE_TRAITS_HPP
#define LMADB_CXX_TYPE_TRAITS_HPP

#include <optional>
#include <tuple>
#include <variant>

namespace lmadb::cxx {

template <template <typename...> typename T, typename U>
struct is_template {
  constexpr static bool value = false;
};

template <template <typename...> typename T, typename ...Args>
struct is_template<T, T<Args...>> {
  constexpr static bool value = true;
};

template <typename T>
constexpr auto is_optional_v = is_template<std::optional, T>::value;

template <typename T>
constexpr auto is_tuple_v = is_template<std::tuple, T>::value;

template <typename T>
constexpr auto is_variant_v = is_template<std::variant, T>::value;

} // namespace lmadb:cxx

#endif // LMADB_CXX_TYPE_TRAITS_HPP
