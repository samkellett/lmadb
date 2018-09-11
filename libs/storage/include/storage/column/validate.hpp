#ifndef LMADB_STORAGE_COLUMN_VALIDATE_HPP
#define LMADB_STORAGE_COLUMN_VALIDATE_HPP

#include <stdexcept>
#include <string>

namespace lmadb {

namespace ast {
struct insert_into;
} // namespace ast

namespace meta {
struct table_desc;
} // namespace meta

namespace storage::column {

class invalid_insert_into : public std::runtime_error {
public:
  invalid_insert_into(std::string what) : std::runtime_error{std::move(what)} {}
};

// validation routines for statements, throws on invalid combinations.
auto validate(const ast::insert_into &insert_into, const meta::table_desc &table_desc)
  -> void;

} // namespace storage::column
} // namespace lmadb

#endif // LMADB_STORAGE_COLUMN_VALIDATE_HPP
