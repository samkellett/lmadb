#ifndef LMADB_CXX_VARIANT_CAT_HPP
#define LMADB_CXX_VARIANT_CAT_HPP

#include <type_traits>
#include <variant>

namespace lmadb::cxx {

template <typename T, typename U>
struct variant_cat;

template <typename T, typename U>
using variant_cat_t = typename variant_cat<T, U>::type;

template <typename ...Ts, typename U>
struct variant_cat<std::variant<Ts...>, std::variant<U>> {
private:
  constexpr static auto is_subset = std::disjunction_v<std::is_same<Ts, U>...>;

public:
  using type = std::conditional_t<is_subset, std::variant<Ts...>, std::variant<Ts..., U>>;
};

template <typename ...Ts, typename U, typename ...Us>
struct variant_cat<std::variant<Ts...>, std::variant<U, Us...>> {
  using type = variant_cat_t<variant_cat_t<std::variant<Ts...>, std::variant<U>>, std::variant<Us...>>;
};

} // namespace lmadb::cxx

#endif // LMADB_CXX_VARIANT_CAT_HPP
