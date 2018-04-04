#ifndef LMADB_PDK_PDK_HPP
#define LMADB_PDK_PDK_HPP

#include "cxx/type_traits.hpp"
#include "cxx/variant_cat.hpp"

#include <boost/lexical_cast.hpp>

#include <array>
#include <cctype>
#include <optional>
#include <string_view>
#include <tuple>
#include <variant>
#include <vector>

namespace lmadb::pdk {

// The following is a collection of tools for creating runtime parsers at compile time.
// It is inspired by Boost.Spirit X3 and https://github.com/lefticus/constexpr_all_the_things

// A parser is any callable type that takes a string_view and returns an optional pair of the matching type
// and the remaining characters to be parser.
using parse_input_t = std::string_view;

template <typename T>
using parse_result_t = std::optional<std::pair<T, std::string_view>>;

// Metafunction to get the underlying parsed type out of a parser.
template <typename P>
using parse_t = typename std::result_of_t<P(parse_input_t)>::value_type::first_type;

// Literal parsers:
// These parsers match compile time constants. They don't return anything (which is represented by a monostate).
//   lit       : matches a set of characters,
//   ilit      : matches a set of characters (case insensitive).
//
// String parsers:
// These parsers take a string_view and return another one.
//   pred<F>   : matches all characters until F returns false,
//   alpha     : matches all alphabetical characters,
//   digit     : matches all numerical characters,
//   space     : matches all whitespace characters.
//
// Transformation parsers:
// These parsers will change the result type of a parser.
//   A / F        : matches parser A and then applies the result to function F,
//   ~A           : matches parser A and discards the result,
//   to<V>        : transforms a parse result into the value V,
//   construct<T> : calls the constructor for T with the parse result.
//
// Number parsers:
// These parsers turn strings into numbers.
//   int32     : matches a 32-bit integer.
//
// Combination parsers:
// These parsers take two parsers and return parsers that must match both. Complex type propagation occurs here
// which is explained further down. These are normally represented by a tuple.
//   A >> B    : matches parser A, immediately followed by parser B,
//   A > B     : matches parser A, then any number of whitespace characters and then parser B.
//
// Optional parsers:
// These parsers always succeed and are represented by an optional.
//   -A        : matches parser A one or zero times.
//
// Alternation parsers:
// Parsers that match one of the parsers given. These are normally represented by a variant.
//   A | B     : matches parser A or parser B (first one wins).
//
// List parsers:
// These parsers return multiple values and are normally represented by a vector.
//   *A        : matches parser A zero or more times,
//   +A        : matches parser A one or more times,
//   A % B     : matches parser A one or more times, interwoven with parser B.
//
// Type propagation rules:
// Some higher order parsers will manipulate the result type.
// Key: ? => monostate, [A] => vector<A>, (A, B) => tuple<A, B>, (A | B) => variant<A, B>, A? => optional<A>
//   ? / F              => F()
//   A / F              => F(A)
//   (A, B) / F         => F(A, B)
//   ~A                 => ?
//   A > B              => (A, B)
//   A > ?              => A
//   ? > B              => B
//   ? > ?              => ?
//   (A, B) > C         => (A, B, C)
//   A > (A, B)         => (A, B, C)
//   (A, B) > (C, D)    => (A, B, C, D)
//   A | A              => A
//   A | B              => (A | B)
//   A | (B | C)        => (A | B | C)
//   (A | B) | C        => (A | B | C)
//   (A | B) | (C | D)  => (A | B | C | D)
//   -A                 => A?
//   -A?                => A?
//   -[A]               => [A]
//   *A                 => [A]
//   +A                 => [A]
//   A % B              => [A]

namespace detail {

template <typename Pred, char ...Cs>
constexpr auto lit = [](parse_input_t in) -> parse_result_t<std::monostate> {
  constexpr auto size = sizeof...(Cs);

  if (in.size() < size) {
    return std::nullopt;
  }

  static constexpr std::array<char, size> needle{Cs...};
  if (std::equal(std::begin(needle), std::end(needle), std::begin(in), Pred{})) {
    return parse_result_t<std::monostate>{{{}, in.substr(needle.size())}};
  } else {
    return std::nullopt;
  }
};

struct tolower {
  auto operator()(char lhs, char rhs) const
  {
    return std::tolower(static_cast<unsigned char>(lhs)) == std::tolower(static_cast<unsigned char>(rhs));
  }
};

} // namespace detail

template <char ...Cs>
constexpr auto lit = detail::lit<std::equal_to<char>, Cs...>;

template <char ...Cs>
constexpr auto ilit = detail::lit<detail::tolower, Cs...>;

namespace literals {

template <typename CharT, CharT ...Cs>
constexpr auto operator""_lit() {
  return lit<Cs...>;
}

template <typename CharT, CharT ...Cs>
constexpr auto operator""_ilit() {
  return ilit<Cs...>;
}

} // namespace literals

template <typename F>
constexpr auto pred = [](parse_input_t in) -> parse_result_t<std::string_view> {
  if (in.empty()) {
    return std::nullopt;
  }

  const auto begin = std::begin(in);
  const auto last = std::find_if_not(begin, std::end(in), F{});
  if (last == begin) {
    return std::nullopt;
  } else {
    const auto distance = std::distance(begin, last);
    return parse_result_t<std::string_view>{{in.substr(0, distance), in.substr(distance)}};
  }
};

namespace detail {

// wrapper to protect against UB.
template <auto F>
struct cctype_wrapper {
  auto operator()(char c) const
  {
    return F(static_cast<unsigned char>(c));
  }
};

} // namespace detail

constexpr auto alpha = pred<detail::cctype_wrapper<static_cast<int(*)(int)>(std::isalpha)>>;
constexpr auto digit = pred<detail::cctype_wrapper<static_cast<int(*)(int)>(std::isdigit)>>;
constexpr auto space = pred<detail::cctype_wrapper<static_cast<int(*)(int)>(std::isspace)>>;

template <typename P, typename F>
constexpr auto operator/(P p, F f)
{
  return [p, f](parse_input_t in) {
    auto r = p(in);

    // three posibilities:
    if constexpr(std::is_same_v<parse_t<P>, std::monostate>) {
      // call f();
      using Ret = decltype(f());
      if (r) {
        return parse_result_t<Ret>{{f(), r->second}};
      } else {
        return parse_result_t<Ret>{};
      }
    } else if constexpr(cxx::is_tuple_v<parse_t<P>>) {
      // call f(get<0>(r->first), ...);
      using Ret = decltype(std::apply(f, r->first));
      if (r) {
        return parse_result_t<Ret>{{std::apply(f, r->first), r->second}};
      } else {
        return parse_result_t<Ret>{};
      }
    } else {
      // call f(r->first);
      using Ret = decltype(f(r->first));
      if (r) {
        return parse_result_t<Ret>{{f(r->first), r->second}};
      } else {
        return parse_result_t<Ret>{};
      }
    }
  };
}

namespace detail {

template <typename T>
const auto from_chars_helper = [](std::string_view str) -> T
{
  // TODO: use std::from_chars when it is available.
  return boost::lexical_cast<T>(str.data(), str.size());
};

} // namespace detail

constexpr auto int32 = digit / detail::from_chars_helper<std::int32_t>;

template <typename P>
constexpr auto operator~(P p)
{
  return p / []([[maybe_unused]] auto ...x) {
    return std::monostate{};
  };
}

template <typename A, typename B>
constexpr auto operator>>(A a, B b)
{
  return [a, b](parse_input_t in) {
    // if both parsers match in sequence returns a tuple of the results and the latest input string.
    const auto try_parse = [a, b](parse_input_t in) {
      using R = std::tuple<parse_t<A>, parse_t<B>, std::string_view>;
      if (auto r1 = a(in)) {
        if (auto r2 = b(r1->second)) {
          return std::optional<R>{R{r1->first, r2->first, r2->second}};
        }
      }

      return std::optional<R>{};
    };

    using Ar = parse_t<A>;
    using Br = parse_t<B>;

    // seven possibilities here.
    if constexpr(std::is_same_v<Ar, std::monostate> && !std::is_same_v<Br, std::monostate>) {
      // lhs is a monostate.
      if (auto r = try_parse(in)) {
        auto &[lhs, rhs, out] = *r;
        return parse_result_t<Br>{{rhs, out}};
      } else {
        return parse_result_t<Br>{};
      }
    } else if constexpr(!std::is_same_v<Ar, std::monostate> && std::is_same_v<Br, std::monostate>) {
      // rhs is a monostate.
      if (auto r = try_parse(in)) {
        auto &[lhs, rhs, out] = *r;
        return parse_result_t<Ar>{{lhs, out}};
      } else {
        return parse_result_t<Ar>{};
      }
    } else if constexpr(std::is_same_v<Ar, std::monostate> && std::is_same_v<Br, std::monostate>) {
      // both are a monostate.
      if (auto r = try_parse(in)) {
        auto &[lhs, rhs, out] = *r;
        return parse_result_t<std::monostate>{{{}, out}};
      } else {
        return parse_result_t<std::monostate>{};
      }
    } else if constexpr(cxx::is_tuple_v<Ar> && !cxx::is_tuple_v<Br>) {
      // lhs is a tuple.
      using R = decltype(std::tuple_cat(std::declval<Ar>(), std::tuple{std::declval<Br>()}));
      if (auto r = try_parse(in)) {
        auto &[lhs, rhs, out] = *r;
        return parse_result_t<R>{{std::tuple_cat(lhs, std::tuple{rhs}), out}};
      } else {
        return parse_result_t<R>{};
      }
    } else if constexpr(!cxx::is_tuple_v<Ar> && cxx::is_tuple_v<Br>) {
      // rhs is a tuple.
      using R = decltype(std::tuple_cat(std::tuple{std::declval<Ar>()}, std::declval<Br>()));
      if (auto r = try_parse(in)) {
        auto &[lhs, rhs, out] = *r;
        return parse_result_t<R>{{std::tuple_cat(std::tuple{lhs}, rhs), out}};
      } else {
        return parse_result_t<R>{};
      }
    } else if constexpr(cxx::is_tuple_v<Ar> && cxx::is_tuple_v<Br>) {
      // both are a tuple.
      using R = decltype(std::tuple_cat(std::declval<Ar>(), std::declval<Br>()));
      if (auto r = try_parse(in)) {
        auto &[lhs, rhs, out] = *r;
        return parse_result_t<R>{{std::tuple_cat(lhs, rhs), out}};
      } else {
        return parse_result_t<R>{};
      }
    } else {
      // every other possibility.
      using R = std::tuple<Ar, Br>;
      if (auto r = try_parse(in)) {
        auto &[lhs, rhs, out] = *r;
        return parse_result_t<R>{{std::make_tuple(lhs, rhs), out}};
      } else {
        return parse_result_t<R>{};
      }
    }
  };
}

template <typename A>
constexpr auto operator-(A a)
{
  return [a](parse_input_t in) {
    using R = parse_t<A>;

    if constexpr(cxx::is_optional_v<R>) {
      if (auto r = a(in)) {
        return r;
      } else {
        return parse_result_t<R>{{{}, in}};
      }
    } else {
      if (auto r = a(in)) {
        return parse_result_t<std::optional<R>>{{r->first, r->second}};
      } else {
        return parse_result_t<std::optional<R>>{{{}, in}};
      }
    }
  };
}

template <typename A, typename B>
constexpr auto operator>(A a, B b)
{
  return a >> ~(-space) >> b;
}

template <typename A, typename B>
constexpr auto operator|(A a, B b)
{
  return [a, b](parse_input_t in) {
    using Ar = parse_t<A>;
    using Br = parse_t<B>;

    // five possibilities for type propagation.
    if constexpr(std::is_same_v<Ar, Br>) {
      // same type, no variant needed.
      if (auto r = a(in)) {
        return r;
      } else if (auto r = b(in)) {
        return r;
      } else {
        return parse_result_t<Ar>{};
      }
    } else if constexpr(cxx::is_variant_v<Ar> && !cxx::is_variant_v<Br>) {
      // left is a variant.
      using R = cxx::variant_cat_t<Ar, std::variant<Br>>;
      if (auto r = a(in)) {
        return parse_result_t<R>{{std::visit([](auto x) -> R { return x; }, r->first), r->second}};
      } else if (auto r = b(in)) {
        return parse_result_t<R>{r};
      } else {
        return parse_result_t<R>{};
      }
    } else if constexpr(!cxx::is_variant_v<Ar> && cxx::is_variant_v<Br>) {
      // right is a variant.
      using R = cxx::variant_cat_t<std::variant<Ar>, Br>;
      if (auto r = a(in)) {
        return parse_result_t<R>{r};
      } else if (auto r = b(in)) {
        return parse_result_t<R>{{std::visit([](auto x) -> R { return x; }, r->first), r->second}};
      } else {
        return parse_result_t<R>{};
      }
    } else if constexpr(cxx::is_variant_v<Ar> && cxx::is_variant_v<Br>) {
      // both are variants.
      using R = cxx::variant_cat_t<Ar, Br>;
      if (auto r = a(in)) {
        return parse_result_t<R>{{std::visit([](auto x) -> R { return x; }, r->first), r->second}};
      } else if (auto r = b(in)) {
        return parse_result_t<R>{{std::visit([](auto x) -> R { return x; }, r->first), r->second}};
      } else {
        return parse_result_t<R>{};
      }
    } else {
      // everything else.
      using R = std::variant<Ar, Br>;
      if (auto r = a(in)) {
        return parse_result_t<R>{{r->first, r->second}};
      } else if (auto r = b(in)) {
        return parse_result_t<R>{{r->first, r->second}};
      } else {
        return parse_result_t<R>{};
      }
    }
  };
}

template <typename A>
constexpr auto operator+(A a)
{
  using Rs = std::vector<parse_t<A>>;

  return [a](parse_input_t in) -> parse_result_t<Rs> {
    Rs values;

    while (auto r = a(in)) {
      values.push_back(r->first);
      in = r->second;
    }

    if (values.empty()) {
      return std::nullopt;
    } else {
      return parse_result_t<Rs>{{values, in}};
    }
  };
}

template <typename A>
constexpr auto operator*(A a)
{
  using Rs = std::vector<parse_t<A>>;

  return [a](parse_input_t in) -> parse_result_t<Rs> {
    Rs values;

    while (auto r = a(in)) {
      values.push_back(r->first);
      in = r->second;
    }

    return parse_result_t<Rs>{{values, in}};
  };
}

template <typename A, typename B>
constexpr auto operator%(A a, B b)
{
  return (a > *(b > a)) / [](auto x, auto xs) {
    // TODO: this is inefficient.
    xs.insert(std::begin(xs), x);
    return xs;
  };
}

template <auto V>
constexpr auto to = []([[maybe_unused]] const auto &...x)
{
  return V;
};

template <typename T>
constexpr auto construct = [](auto&& ...x)
{
  return T{std::forward<decltype(x)>(x)...};
};

} // namespace lmadb::pdk

#endif // LMADB_PDK_PDK_HPP