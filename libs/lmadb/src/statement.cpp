#include "statement.hpp"

#include "ast/parse_statement.hpp"

#include "storage/column/insert.hpp"
#include "storage/column/validate.hpp"

#include "meta/reader.hpp"
#include "meta/writer.hpp"

namespace lmadb {

namespace {

class statement_visitor {
public:
  explicit statement_visitor(const cxx::filesystem::path &db)
  : db_{db}
  {
  }

  auto operator()(const ast::create_table &create_table) const -> statement::generator
  {
    meta::writer md{db_};
    md.create_table(create_table);

    // we're done with nothing to return.
    co_yield nullptr;
  }

  auto operator()(const ast::insert_into &insert_into) const -> statement::generator
  {
    // load metadata for table.
    const auto table_desc{[&] {
      const meta::reader md{db_};
      return md.find_table(insert_into.table);
    }()};

    // validate statement.
    storage::column::validate(insert_into, table_desc);

    // insert record.
    storage::column::insert(db_, insert_into, table_desc);

    // we're done with nothing to return.
    co_yield nullptr;
  }

  auto operator()([[maybe_unused]] const ast::select &select) const -> statement::generator
  {
    throw std::logic_error{"unimplemented."};
  }

private:
  const cxx::filesystem::path &db_;
};

auto make_statement(const cxx::filesystem::path &db, const std::string_view sql)
  -> statement::generator
{
  if (auto stmt = ast::parse_statement(sql)) {
    // create a coroutine that will yield every row this query returns (if any).
    return std::visit(statement_visitor{db}, *stmt);
  } else {
    // TODO: better error handling when parsing fails.
    throw invalid_sql_error{"invalid SQL: '{}'.", sql};
  }
}

} // unnamed namespace

statement::statement(const cxx::filesystem::path &db, std::string_view sql_, std::string &error_)
: sql{std::move(sql_)},
  row{make_statement(db, sql)},
  // the coroutine remains unstarted until begin is called on the first call to lmadb_step.
  it{std::end(row)},
  error{error_}
{
}

} // namespace lmadb
