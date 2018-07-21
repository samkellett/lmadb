#include "connection.hpp"

#include "meta/reader.hpp"

#include "statement.hpp"

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

auto connection::create_statement(std::string_view sql)
  -> std::unique_ptr<statement>
{
  return std::make_unique<statement>(path_, std::move(sql));
}

auto connection::list_tables() const -> std::vector<std::string>
{
  const meta::reader reader{path_};
  return reader.list_tables();
}

} // namespace lmadb
