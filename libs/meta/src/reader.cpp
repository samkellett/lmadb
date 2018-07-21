#include "meta/reader.hpp"

#include "table_metadata_map.hpp"

#include <boost/range/adaptor/map.hpp>
#include <boost/range/adaptor/transformed.hpp>

#include <boost/range/algorithm/copy.hpp>

namespace lmadb::meta {

reader::reader(const cxx::filesystem::path &db)
: db_{db}
{
}

auto reader::list_tables() const -> std::vector<std::string>
{
  using boost::adaptors::map_keys;
  using boost::adaptors::transformed;

  table_metadata_map tables{db_ / "db.tables"};

  auto names = tables | map_keys | transformed([](const auto &key) { return key.string(); });

  std::vector<std::string> table_list;
  table_list.reserve(tables.size());
  boost::copy(names, std::back_inserter(table_list));
  return table_list;
}

} // namespace lmadb::meta
