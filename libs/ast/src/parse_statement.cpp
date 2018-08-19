#include "ast/parse_statement.hpp"

#include "pdk/pdk.hpp"

namespace lmadb {
namespace parser {

using namespace lmadb::pdk;
using namespace lmadb::pdk::literals;

constexpr auto column_name
  = alpha;
constexpr auto table_name
  = alpha;

constexpr auto type_name
  = "int8"_ilit / to<ast::type::int64>;

constexpr auto column_def
  = (column_name > type_name) / construct<ast::column_def>;

constexpr auto create_table_statement
  = ("create"_ilit > "table"_ilit > table_name > "("_lit > (column_def % ","_lit) > ")"_lit)
  / construct<ast::create_table>;

constexpr auto sql_statement
  = create_table_statement > ";"_lit;

} // namespace parser

namespace ast {

auto parse_statement(const std::string_view sql) -> std::optional<ast::sql_statement>
{
  if (auto result = parser::sql_statement(sql)) {
    // TODO: don't discard the unparsed input.
    return result->first;
  } else {
    // TODO: return parse error information.
    return std::nullopt;
  }
}

} // namespace ast
} // namespace lmadb
