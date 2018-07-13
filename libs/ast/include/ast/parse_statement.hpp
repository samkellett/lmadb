#ifndef LMADB_PARSE_STATEMENT_HPP
#define LMADB_PARSE_STATEMENT_HPP

#include "ast/ast.hpp"

#include <optional>
#include <string_view>

namespace lmadb::ast {

auto parse_statement(std::string_view) -> std::optional<ast::sql_statement>;

} // namespace lmadb::ast

#endif // LMADB_PARSE_STATEMENT_HPP
