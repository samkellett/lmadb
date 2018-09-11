#ifndef LMADB_META_TABLE_METADATA_MAP
#define LMADB_META_TABLE_METADATA_MAP

#include "cxx/adt/const_str.hpp"
#include "cxx/adt/persistent_flat_map.hpp"

namespace lmadb::meta {

class table_exists_error : public std::runtime_error {
public:
  table_exists_error(std::string what) : std::runtime_error{std::move(what)} {}
};

// TODO: make these newtypes.
using table_id = std::uint32_t;
using table_name = cxx::const_str<64>;

struct table_metadata {
  table_id id;
};

using table_metadata_map = cxx::persistent_flat_map<table_name, table_metadata>;

} // namespace lmadb::meta

#endif // LMADB_META_TABLE_METADATA_MAP
