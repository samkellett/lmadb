#ifndef LMADB_STATEMENT_HPP
#define LMADB_STATEMENT_HPP

#include "cxx/define_exception.hpp"
#include "cxx/filesystem.hpp"

#include <cppcoro/generator.hpp>

#include <functional>
#include <string>
#include <string_view>

namespace lmadb {

LMADB_DEFINE_EXCEPTION(invalid_sql_error);

// a statement frame contains the coroutine that will generate rows and stays alive for the
// duration of the query.
struct statement {
  // TODO: for now we don't support returning data.
  using generator = cppcoro::generator<void *>;

  statement(const cxx::filesystem::path &db, std::string_view sql, std::string &error);

  // take a copy of the query string as we cannot gurantee lifetime of the the one given.
  std::string sql;
  // the top-level coroutine that will produce any data produced by the query.
  generator row;
  // whee we currently are.
  generator::iterator it;
  // a buffer owned by the connection for exposing error messages through the public api.
  std::string &error;
};

} // namespace lmadb

#endif // LMADB_STATEMENT_HPP
