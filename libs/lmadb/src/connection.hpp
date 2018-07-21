#ifndef LMADB_CONNECTION_HPP
#define LMADB_CONNECTION_HPP

#include "cxx/filesystem.hpp"

#include <string_view>

namespace lmadb {

class statement;

class connection {
public:
  explicit connection(cxx::filesystem::path path) noexcept;

  auto create_statement(std::string_view sql) -> std::unique_ptr<statement>;
  auto list_tables() const -> std::vector<std::string>;

private:
  cxx::filesystem::path path_;
};

} // namespace lmadb

#endif // LMADB_CONNECTION_HPP
