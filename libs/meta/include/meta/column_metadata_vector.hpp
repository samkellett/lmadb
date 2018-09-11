#ifndef LMADB_META_COLUMN_METADATA_VECTOR
#define LMADB_META_COLUMN_METADATA_VECTOR

#include "ast/ast.hpp"

#include "cxx/adt/const_str.hpp"
#include "cxx/adt/persistent_vector.hpp"

namespace lmadb::meta {

class column_exists_error : public std::runtime_error {
public:
  column_exists_error(std::string what) : std::runtime_error{std::move(what)} {}
};

// TODO: make these newtypes.
using column_name = cxx::const_str<64>;

struct column_metadata {
  column_name name;
  lmadb::ast::type type;
};

using column_metadata_vector = cxx::persistent_vector<column_metadata>;

} // namespace lmadb::meta

#endif // LMADB_META_COLUMN_METADATA_VECTOR
