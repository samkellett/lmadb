#include "meta/writer.hpp"

#include "meta/column_metadata_vector.hpp"
#include "meta/table_metadata_map.hpp"

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
  table_id id(tables.size() + 1);
  auto [_, inserted] = tables.insert({table_name, {id}});
  if (!inserted) {
    throw table_exists_error{fmt::format("cannot insert table '{}', already exists.", table_name)};
  }

  try {
    // now insert the columns into a new map.
    column_metadata_vector columns{db_ / fmt::format("{}.columns", table_name)};

    // TODO: this is quadratic.
    for (const auto &column : create_table.columns) {
      // check the column doesn't already exist.
      auto it = std::find_if(std::begin(columns), std::end(columns), [&](const auto &c) {
        return c.name == column.name;
      });
      if (it != std::end(columns)) {
        throw column_exists_error{fmt::format("cannot create table, column '{}' is not unique.", column.name)};
      }

      columns.push_back({column.name, column.type});
    }
  } catch([[maybe_unused]] const std::exception &e) {
    // TODO: what to do if this fails?
    [[maybe_unused]] const auto erased{tables.erase(table_name)};
    assert(erased == 1);

    throw;
  }
}

} // namespace lmadb::meta
