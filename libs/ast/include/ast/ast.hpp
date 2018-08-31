#ifndef LMADB_AST_HPP
#define LMADB_AST_HPP

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

// TODO: make this process less manual.
inline auto operator==(const column_def &lhs, const column_def &rhs) -> bool
{
  return lhs.name == rhs.name && lhs.type == rhs.type;
}

inline auto operator==(const create_table &lhs, const create_table &rhs) -> bool
{
  return lhs.name == rhs.name && lhs.columns == rhs.columns;
}

inline auto operator==(const insert_into &lhs, const insert_into &rhs) -> bool
{
  return lhs.table == rhs.table && lhs.values == rhs.values;
}

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
