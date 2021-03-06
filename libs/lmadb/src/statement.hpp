#ifndef LMADB_STATEMENT_HPP
#define LMADB_STATEMENT_HPP

#include "cxx/define_exception.hpp"
#include "cxx/filesystem.hpp"

#include "ast/ast.hpp"

#include <functional>
#include <string>
#include <string_view>

namespace lmadb {

LMADB_DEFINE_EXCEPTION(invalid_sql_error);

enum class step_status : bool {
  row,
  done
};

class statement {
public:
  statement(const cxx::filesystem::path &db,
            std::string_view sql,
            std::function<void(std::string_view)> set_error_callback);

  auto step() -> step_status;

  auto set_error(std::string_view msg) -> void;

private:
  const cxx::filesystem::path &db_;

  std::string query_string_;
  ast::sql_statement stmt_;

  // TODO: make this a small_function
  std::function<void(std::string_view)> set_error_callback_;
};

} // namespace lmadb

#endif // LMADB_STATEMENT_HPP
