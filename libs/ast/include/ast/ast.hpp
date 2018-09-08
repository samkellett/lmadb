#ifndef LMADB_AST_HPP
#define LMADB_AST_HPP

#include "cxx/to_tuple.hpp"

#include <string_view>
#include <variant>
#include <vector>

namespace lmadb {
namespace ast {

enum class type {
  int64
};

struct column_def {
  std::string_view name;
  type type;
};

using literal_value = std::variant<
  std::int64_t
>;

using expr = std::variant<
  literal_value
>;

struct create_table {
  std::string_view name;
  std::vector<column_def> columns;
};

struct insert_into {
  std::string_view table;
  std::vector<expr> values;
};

using sql_statement = std::variant<
  create_table,
  insert_into
>;

#define LMADB_DEFINE_EQ_OPS(ty)                                 \
  inline auto operator==(const ty &lhs, const ty &rhs) -> bool { \
    return cxx::to_tuple(lhs) == cxx::to_tuple(rhs);             \
  }                                                              \
  inline auto operator!=(const ty &lhs, const ty &rhs) -> bool { \
    return !(lhs == rhs);                                        \
  }

LMADB_DEFINE_EQ_OPS(column_def)
LMADB_DEFINE_EQ_OPS(create_table)
LMADB_DEFINE_EQ_OPS(insert_into)

#undef LMADB_DEFINE_EQ_OPS

// TODO: some sort of printing (stream operators?).

} // namespace ast

namespace cxx {

inline auto to_string_view(ast::type type) -> std::string_view
{
  switch(type) {
    // TODO: this sucks, should rename the int64 enum.
    case ast::type::int64: return "int8";
  }
}

} // namespace cxx
} // namespace lmadb

#endif // LMADB_AST_HPP
