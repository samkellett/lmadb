#ifndef LMADB_PARSE_STATEMENT_HPP
#define LMADB_PARSE_STATEMENT_HPP

#include "ast.hpp"

#include <optional>
#include <string_view>

namespace lmadb {

auto parse_statement(std::string_view) -> std::optional<ast::sql_statement>;

} // namespace lmadb

#endif // LMADB_PARSE_STATEMENT_HPP
