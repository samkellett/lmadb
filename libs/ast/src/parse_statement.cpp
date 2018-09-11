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
  = "int8"_ilit / to<ast::type::int64>
  | "boolean"_ilit / to<ast::type::bool_>;

constexpr auto column_def
  = (column_name > type_name) / construct<ast::column_def>;

constexpr auto bool_
  = "true"_ilit / to<true>
  | "false"_ilit / to<false>
  ;

constexpr auto literal_value
  = (bool_ | int64)
  / construct<ast::literal_value>;

constexpr auto expr
  = literal_value
  / construct<ast::expr>;

constexpr auto create_table_statement
  = ("create"_ilit > "table"_ilit > table_name > "("_lit > (column_def % ","_lit) > ")"_lit)
  / construct<ast::create_table>;

constexpr auto insert_into_statement
  = ("insert"_ilit > "into"_ilit > table_name > "values"_ilit > "("_ilit > (expr % ","_lit) > ")"_lit)
  / construct<ast::insert_into>;

constexpr auto sql_statement
  = create_table_statement
  | insert_into_statement
  ;

} // namespace parser

namespace ast {

auto parse_statement(std::string_view sql) -> std::optional<ast::sql_statement>
{
  // strip initial whitespace.
  sql = sql.substr(sql.find_first_not_of(" \n\r\t"));
  if (const auto result = parser::sql_statement(sql)) {
    // TODO: don't discard the unparsed input.
    return result->first;
  } else {
    // TODO: return parse error information.
    return std::nullopt;
  }
}

} // namespace ast
} // namespace lmadb
