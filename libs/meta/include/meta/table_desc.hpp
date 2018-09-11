#ifndef LMADB_META_TABLE_DESC_HPP
#define LMADB_META_TABLE_DESC_HPP

#include "meta/column_metadata_vector.hpp"
#include "meta/table_metadata_map.hpp"

namespace lmadb::meta {

struct table_desc {
  table_metadata table;
  column_metadata_vector columns;
};

} // namespace lmadb::meta

#endif // LMADB_META_TABLE_DESC_HPP
