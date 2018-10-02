#ifndef LMADB_STORAGE_COLUMN_COLUMN_VECTOR
#define LMADB_STORAGE_COLUMN_COLUMN_VECTOR

#include "cxx/adt/persistent_vector.hpp"

namespace lmadb::storage::column {

template <typename T>
using column_vector = cxx::persistent_vector<T>;

} // namespace lmadb::storage::column

#endif // LMADB_STORAGE_COLUMN_COLUMN_VECTOR
