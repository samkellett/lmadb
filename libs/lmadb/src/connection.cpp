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
  return std::make_unique<statement>(path_, std::move(sql), error_);
}

auto connection::list_tables() const -> std::vector<std::string>
{
  const meta::reader reader{path_};
  return reader.list_tables();
}

auto connection::describe_table(std::string_view table) const
  -> std::vector<std::pair<std::string, std::string_view>>
{
  const meta::reader reader{path_};
  return reader.describe_table(std::move(table));
}

auto connection::error() const -> const std::string &
{
  return error_;
}

auto connection::set_error(std::string_view msg) -> void
{
  error_ = msg;
}

} // namespace lmadb
