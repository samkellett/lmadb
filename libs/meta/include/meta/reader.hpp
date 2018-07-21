#ifndef LMADB_META_READER_HPP
#define LMADB_META_READER_HPP

#include "cxx/filesystem.hpp"

namespace lmadb::meta {

class reader {
public:
  explicit reader(const cxx::filesystem::path &db);

  auto list_tables() const -> std::vector<std::string>;

private:
  const cxx::filesystem::path &db_;
};


} // namespace lmadb::meta

#endif // LMADB_META_READER_HPP