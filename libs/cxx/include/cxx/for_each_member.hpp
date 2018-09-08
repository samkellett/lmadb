#ifndef LMADB_CXX_FOR_EACH_MEMBER_HPP
#define LMADB_CXX_FOR_EACH_MEMBER_HPP

#include "to_tuple.hpp"

#include <tuple>

namespace lmadb::cxx {

template <typename Struct, typename UnaryFunction>
auto for_each_member(Struct &&object, const UnaryFunction &fn)
{
  std::apply([&](auto&& ...xs) {
    (fn(std::forward<decltype(xs)>(xs)), ...);
  }, to_tuple(std::forward<Struct>(object)));
}

} // namespace lmadb::cxx

#endif // LMADB_CXX_FOR_EACH_MEMBER_HPP
