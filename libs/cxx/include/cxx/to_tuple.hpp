#ifndef LMADB_CXX_TO_TUPLE_HPP
#define LMADB_CXX_TO_TUPLE_HPP

#include "type_traits.hpp"

#include <tuple>

namespace lmadb::cxx {

// TODO: make sure this works efficiently (eg, no unnecessary copies).
template<class T>
auto to_tuple(T&& object) noexcept
{
  using Td = std::decay_t<T>;

  // add more predicates here as needed.
  if constexpr(is_braces_constructible_with_arity_v<Td, 5>) {
    auto&& [p1, p2, p3, p4, p5] = std::forward<T>(object);
    return std::forward_as_tuple(std::forward<decltype(p1)>(p1),
                                 std::forward<decltype(p2)>(p2),
                                 std::forward<decltype(p3)>(p3),
                                 std::forward<decltype(p4)>(p4),
                                 std::forward<decltype(p5)>(p5));
  } else if constexpr(is_braces_constructible_with_arity_v<Td, 4>) {
    auto&& [p1, p2, p3, p4] = std::forward<T>(object);
    return std::forward_as_tuple(std::forward<decltype(p1)>(p1),
                                 std::forward<decltype(p2)>(p2),
                                 std::forward<decltype(p3)>(p3),
                                 std::forward<decltype(p4)>(p4));
  } else if constexpr(is_braces_constructible_with_arity_v<Td, 3>) {
    auto&& [p1, p2, p3] = std::forward<T>(object);
    return std::forward_as_tuple(std::forward<decltype(p1)>(p1),
                                 std::forward<decltype(p2)>(p2),
                                 std::forward<decltype(p3)>(p3));
  } else if constexpr(is_braces_constructible_with_arity_v<Td, 2>) {
    auto&& [p1, p2] = std::forward<T>(object);
    return std::forward_as_tuple(std::forward<decltype(p1)>(p1),
                                 std::forward<decltype(p2)>(p2));
  } else if constexpr(is_braces_constructible_with_arity_v<Td, 1>) {
    auto&& [p1] = std::forward<T>(object);
    return std::forward_as_tuple(std::forward<decltype(p1)>(p1));
  } else {
    return std::forward_as_tuple();
  }
}

} // namespace lmadb::cxx

#endif // LMADB_CXX_TO_TUPLE_HPP
