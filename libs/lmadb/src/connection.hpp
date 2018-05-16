#ifndef LMADB_CONNECTION_HPP
#define LMADB_CONNECTION_HPP

#include "cxx/filesystem.hpp"

namespace lmadb {

class connection {
public:
  explicit connection(cxx::filesystem::path path) noexcept;

private:
  cxx::filesystem::path path_;
};

inline
connection::connection(cxx::filesystem::path path) noexcept
: path_{std::move(path)}
{
}

} // namespace lmadb

#endif // LMADB_CONNECTION_HPP
