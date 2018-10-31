#ifndef LMADB_CONNECTION_HPP
#define LMADB_CONNECTION_HPP

#include "cxx/filesystem.hpp"

#include <string_view>

namespace lmadb {

struct statement;

class connection {
public:
  explicit connection(cxx::filesystem::path path) noexcept;

  auto create_statement(std::string_view sql) -> std::unique_ptr<statement>;

  auto describe_table(std::string_view table) const -> std::vector<std::pair<std::string, std::string_view>>;
  auto list_tables() const -> std::vector<std::string>;

  auto error() const -> const std::string &;
  auto set_error(std::string_view msg) -> void;

private:
  cxx::filesystem::path path_;
  std::string error_;
};

} // namespace lmadb

#endif // LMADB_CONNECTION_HPP
