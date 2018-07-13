#ifndef LMADB_STATEMENT_HPP
#define LMADB_STATEMENT_HPP

#include "cxx/filesystem.hpp"

#include "ast/ast.hpp"

#include <string>
#include <string_view>

namespace lmadb {

class invalid_sql_error : public std::runtime_error {
public:
  invalid_sql_error(std::string what) : std::runtime_error{std::move(what)} {}
};

enum class step_status : bool {
  row,
  done
};

class statement {
public:
  statement(const cxx::filesystem::path &db, std::string_view sql);

  auto step() -> step_status;

private:
  const cxx::filesystem::path &db_;

  std::string query_string_;
  ast::sql_statement stmt_;
};

} // namespace lmadb

#endif // LMADB_STATEMENT_HPP
