#ifndef LMADB_STORAGE_COLUMN_VALIDATE_HPP
#define LMADB_STORAGE_COLUMN_VALIDATE_HPP

#include "cxx/define_exception.hpp"

namespace lmadb {

namespace ast {
struct insert_into;
} // namespace ast

namespace meta {
struct table_desc;
} // namespace meta

namespace storage::column {

LMADB_DEFINE_EXCEPTION(invalid_insert_into);

// validation routines for statements, throws on invalid combinations.
auto validate(const ast::insert_into &insert_into, const meta::table_desc &table_desc)
  -> void;

} // namespace storage::column
} // namespace lmadb

#endif // LMADB_STORAGE_COLUMN_VALIDATE_HPP
