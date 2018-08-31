#include "statement.hpp"

#include "ast/parse_statement.hpp"

#include "meta/writer.hpp"

#include <fmt/format.h>

namespace lmadb {

namespace {

class step_visitor {
public:
  explicit step_visitor(const cxx::filesystem::path &db)
  : db_{db}
  {
  }

  auto operator()(const ast::create_table &create_table)
  {
    meta::writer md{db_};
    md.create_table(create_table);
    return step_status::done;
  }

private:
  const cxx::filesystem::path &db_;
};

} // unnamed namespace

statement::statement(const cxx::filesystem::path &db,
                     std::string_view sql,
                     std::function<void(std::string_view)> set_error_callback)
: db_{db},
  query_string_{sql},
  stmt_{},
  set_error_callback_{std::move(set_error_callback)}
{
  // TODO: better error handling when parsing fails.
  if (auto stmt = ast::parse_statement(query_string_)) {
    stmt_ = *std::move(stmt);
  } else {
    throw invalid_sql_error{fmt::format("invalid SQL: '{}'.", query_string_)};
  }
}

auto statement::step() -> step_status
{
  return std::visit(step_visitor{db_}, stmt_);
}

auto statement::set_error(const std::string_view error) -> void
{
  set_error_callback_(error);
}

} // namespace lmadb
