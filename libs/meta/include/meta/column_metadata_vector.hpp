#ifndef LMADB_META_COLUMN_METADATA_VECTOR
#define LMADB_META_COLUMN_METADATA_VECTOR

#include "ast/ast.hpp"

#include "cxx/adt/const_str.hpp"
#include "cxx/adt/persistent_vector.hpp"
#include "cxx/define_exception.hpp"

namespace lmadb::meta {

LMADB_DEFINE_EXCEPTION(column_exists_error);

// TODO: make these newtypes.
using column_name = cxx::const_str<64>;

struct column_metadata {
  column_name name;
  lmadb::ast::type type;
};

using column_metadata_vector = cxx::persistent_vector<column_metadata>;

} // namespace lmadb::meta

#endif // LMADB_META_COLUMN_METADATA_VECTOR
