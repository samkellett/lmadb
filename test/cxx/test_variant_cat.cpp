#include "cxx/variant_cat.hpp"

#include <type_traits>

namespace cxx = lmadb::cxx;

template <typename ...Args>
using var = std::variant<Args...>;

static_assert(std::is_same_v<cxx::variant_cat_t<var<int>, var<int>>, var<int>>);
static_assert(std::is_same_v<cxx::variant_cat_t<var<int>, var<bool>>, var<int, bool>>);

static_assert(std::is_same_v<cxx::variant_cat_t<var<int, bool>, var<bool>>, var<int, bool>>);
static_assert(std::is_same_v<cxx::variant_cat_t<var<bool, int>, var<bool>>, var<bool, int>>);
static_assert(std::is_same_v<cxx::variant_cat_t<var<bool, int>, var<char>>, var<bool, int, char>>);

static_assert(std::is_same_v<cxx::variant_cat_t<var<bool>, var<int, bool>>, var<bool, int>>);
static_assert(std::is_same_v<cxx::variant_cat_t<var<bool>, var<bool, int>>, var<bool, int>>);
static_assert(std::is_same_v<cxx::variant_cat_t<var<char>, var<bool, int>>, var<char, bool, int>>);

static_assert(std::is_same_v<cxx::variant_cat_t<var<int, bool, char>, var<bool>>, var<int, bool, char>>);
static_assert(std::is_same_v<cxx::variant_cat_t<var<int, char, bool>, var<bool>>, var<int, char, bool>>);
static_assert(std::is_same_v<cxx::variant_cat_t<var<char, int, bool>, var<bool>>, var<char, int, bool>>);
static_assert(std::is_same_v<cxx::variant_cat_t<var<char, int, bool>, var<float>>, var<char, int, bool, float>>);

static_assert(std::is_same_v<cxx::variant_cat_t<var<char, int>, var<char, int>>, var<char, int>>);
static_assert(std::is_same_v<cxx::variant_cat_t<var<char, int>, var<int, char>>, var<char, int>>);

static_assert(std::is_same_v<cxx::variant_cat_t<var<char, int>, var<char, float>>, var<char, int, float>>);
static_assert(std::is_same_v<cxx::variant_cat_t<var<char, int>, var<float, char>>, var<char, int, float>>);

static_assert(std::is_same_v<cxx::variant_cat_t<var<char, int>, var<bool, float>>, var<char, int, bool, float>>);

int main()
{
  return 0;
}
