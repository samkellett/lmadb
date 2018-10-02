#include "meta/reader.hpp"

#include "meta/column_metadata_vector.hpp"
#include "meta/table_metadata_map.hpp"

#include <boost/range/adaptor/map.hpp>
#include <boost/range/adaptor/transformed.hpp>

#include <boost/range/algorithm/copy.hpp>

#include <fmt/format.h>

namespace lmadb::meta {

reader::reader(const cxx::filesystem::path &db)
: db_{db}
{
}

auto reader::describe_table(std::string_view table) const
  -> std::vector<std::pair<std::string, std::string_view>>
{
  using boost::adaptors::transformed;

  const table_metadata_map tables{db_ / "db.tables"};
  const auto it = tables.find(table);
  if (it == std::end(tables)) {
    throw table_exists_error{fmt::format("unknown table '{}'.", table)};
  }

  const column_metadata_vector columns{db_ / fmt::format("{}.columns", it->second.id)};

  auto desc = columns | transformed([](const auto &column) {
    return std::make_pair(column.name.string(), cxx::to_string_view(column.type));
  });

  std::vector<std::pair<std::string, std::string_view>> table_desc;
  table_desc.reserve(columns.size());
  boost::copy(desc, std::back_inserter(table_desc));
  return table_desc;
}

auto reader::list_tables() const -> std::vector<std::string>
{
  using boost::adaptors::map_keys;
  using boost::adaptors::transformed;

  const table_metadata_map tables{db_ / "db.tables"};

  auto names = tables | map_keys | transformed([](const auto &key) { return key.string(); });

  std::vector<std::string> table_list;
  table_list.reserve(tables.size());
  boost::copy(names, std::back_inserter(table_list));
  return table_list;
}

auto reader::find_table(const std::string_view table) const -> table_desc
{
  const table_metadata_map tables{db_ / "db.tables"};
  const auto it = tables.find(table);
  if (it == std::end(tables)) {
    throw table_exists_error{fmt::format("unknown table '{}'.", table)};
  }

  return {
    it->second,
    column_metadata_vector{db_ / fmt::format("{}.columns", it->second.id)}
  };
}

} // namespace lmadb::meta
