#ifndef LMADB_META_WRITER_HPP
#define LMADB_META_WRITER_HPP

#include "ast/ast.hpp"

#include "cxx/filesystem.hpp"

namespace lmadb::meta {

class table_exists_error : public std::runtime_error {
public:
  table_exists_error(std::string what) : std::runtime_error{std::move(what)} {}
};

class column_exists_error : public std::runtime_error {
public:
  column_exists_error(std::string what) : std::runtime_error{std::move(what)} {}
};

// class that controls write access to the metadata.
class writer {
public:
  explicit writer(const cxx::filesystem::path &db);

  auto create_table(const ast::create_table &create_table) -> void;

private:
  const cxx::filesystem::path &db_;
};

} // namespace lmadb::meta

#endif // LMADB_META_WRITER_HPP
