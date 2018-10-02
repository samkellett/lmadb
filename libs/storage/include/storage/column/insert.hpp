#ifndef LMADB_STORAGE_COLUMN_INSERT_HPP
#define LMADB_STORAGE_COLUMN_INSERT_HPP

#include "cxx/filesystem.hpp"

namespace lmadb {

namespace ast {
struct insert_into;
} // namespace ast

namespace meta {
struct table_desc;
} // namespace meta

namespace storage::column {

// insert a row into a table.
auto insert(const cxx::filesystem::path &db,
            const ast::insert_into &insert_into,
            const meta::table_desc &table_desc)
  -> void;

} // namespace storage::column
} // namespace lmadb

#endif // LMADB_STORAGE_COLUMN_INSERT_HPP
