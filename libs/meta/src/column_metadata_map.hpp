#ifndef LMADB_META_COLUMN_METADATA_MAP
#define LMADB_META_COLUMN_METADATA_MAP

#include "ast/ast.hpp"

#include "cxx/adt/const_str.hpp"
#include "cxx/adt/persistent_flat_map.hpp"

namespace lmadb::meta {

// TODO: make these newtypes.
using column_id = std::uint32_t;
using column_name = cxx::const_str<64>;

struct column_metadata {
  column_id id;
  lmadb::ast::type type;
};

using column_metadata_map = cxx::persistent_flat_map<column_name, column_metadata>;

} // namespace lmadb::meta

#endif // LMADB_META_COLUMN_METADATA_MAP
