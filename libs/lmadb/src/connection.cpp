#include "connection.hpp"

namespace lmadb {

connection::connection(cxx::filesystem::path path) noexcept
: path_{std::move(path)}
{
  // create the root folder if this is a connection to a new database.
  if (!cxx::filesystem::exists(path_)) {
    cxx::filesystem::create_directory(path_);
  } else {
    // TODO: verify path points to a lmadb database.
  }
}

} // namespace lmadb
