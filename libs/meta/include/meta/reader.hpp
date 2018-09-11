#ifndef LMADB_META_READER_HPP
#define LMADB_META_READER_HPP

#include "cxx/filesystem.hpp"

// #include "meta/column_metadata_vector.hpp"
#include "meta/table_desc.hpp"

namespace lmadb::meta {

class reader {
public:
  explicit reader(const cxx::filesystem::path &db);

  auto describe_table(std::string_view table) const -> std::vector<std::pair<std::string, std::string_view>>;
  auto list_tables() const -> std::vector<std::string>;

  // TODO: return some sort of zero-copy view of the persistent map.
  auto find_table(std::string_view table) const -> table_desc;

private:
  const cxx::filesystem::path &db_;
};

} // namespace lmadb::meta

#endif // LMADB_META_READER_HPP
