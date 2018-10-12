#ifndef LMADB_META_TABLE_METADATA_MAP
#define LMADB_META_TABLE_METADATA_MAP

#include "cxx/adt/const_str.hpp"
#include "cxx/adt/persistent_flat_map.hpp"
#include "cxx/define_exception.hpp"

namespace lmadb::meta {

LMADB_DEFINE_EXCEPTION(table_exists_error);

// TODO: make these newtypes.
using table_id = std::uint32_t;
using table_name = cxx::const_str<64>;

struct table_metadata {
  table_id id;
};

using table_metadata_map = cxx::persistent_flat_map<table_name, table_metadata>;

} // namespace lmadb::meta

#endif // LMADB_META_TABLE_METADATA_MAP
