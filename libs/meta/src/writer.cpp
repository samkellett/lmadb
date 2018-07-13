#include "meta/writer.hpp"

#include "column_metadata_map.hpp"
#include "table_metadata_map.hpp"

#include <fmt/format.h>

namespace lmadb::meta {

writer::writer(const cxx::filesystem::path &db)
: db_{db}
{
}

auto writer::create_table(const ast::create_table &create_table) -> void
{
  const auto &table_name{create_table.name};

  // open the persistent map that stores the table metadata.
  table_metadata_map tables{db_ / "db.tables"};
  // TODO: this id generation won't work when we support dropping tables.
  auto [_, inserted] = tables.insert({table_name, tables.size() + 1});
  if (!inserted) {
    throw table_exists_error{fmt::format("cannot insert table '{}', already exists.", table_name)};
  }

  try {
    // now insert the columns into a new map.
    column_metadata_map columns{db_ / fmt::format("{}.columns", table_name)};
    for (const auto &column : create_table.columns) {
      const column_id id(columns.size() + 1);
      auto [_, inserted] = columns.insert({column.name, {id, column.type}});
      if (!inserted) {
        throw column_exists_error{fmt::format("cannot create table, column '{}' is not unique.", column.name)};
      }
    }
  } catch(const std::exception &e) {
    // TODO: what to do if this fails?
    const auto erased{tables.erase(table_name)};
    assert(erased == 1);

    throw;
  }
}

} // namespace lmadb::meta
