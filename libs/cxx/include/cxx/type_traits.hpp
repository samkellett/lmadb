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

namespace detail {

template <std::size_t>
struct _ {
  template<class T>
  constexpr operator T(); // non explicit
};

template <typename T, typename... Ts>
auto is_braces_constructible(int)
  -> decltype(void(T{std::declval<Ts>()...}), std::true_type{});

template <typename, typename...>
auto is_braces_constructible(...)
  -> std::false_type;

template <typename T, typename I>
struct is_braces_constructible_with_arity;

template <typename T, std::size_t... Is>
struct is_braces_constructible_with_arity<T, std::index_sequence<Is...>>
  : decltype(is_braces_constructible<T, _<Is>...>(0)) {};

} // namespace detail

template <typename T, std::size_t N>
inline constexpr auto is_braces_constructible_with_arity_v =
  detail::is_braces_constructible_with_arity<T, std::make_index_sequence<N>>::value;

} // namespace lmadb:cxx

#endif // LMADB_CXX_TYPE_TRAITS_HPP
