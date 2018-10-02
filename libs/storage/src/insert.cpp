#include "storage/column/insert.hpp"

#include "ast/ast.hpp"

#include "meta/table_desc.hpp"

#include "storage/column/column_vector.hpp"

#include <boost/range/algorithm/find_if.hpp>

namespace lmadb::storage::column {

// insert a row into a table.
auto insert(const cxx::filesystem::path &db,
            const ast::insert_into &insert_into,
            const meta::table_desc &table_desc)
  -> void
{
  // for each column find or create the correct column file.
  assert(table_desc.columns.size() == insert_into.values.size());
  for (auto id{0ul}; id < table_desc.columns.size(); ++id) {
    std::visit([&](const auto &x) {
      using type = std::decay_t<decltype(x)>;
      if constexpr(std::is_same<type, ast::literal_value>{}) {
        std::visit([&](const auto &lit_value) {
          using lit_value_type = std::decay_t<decltype(lit_value)>;

          // open up column file and append data to it.
          column_vector<lit_value_type> column{db / fmt::format("{}.{}.data", table_desc.table.id, id)};
          column.push_back(lit_value);
        }, x);
      } else {
        // TODO: why won't this static assert work?
        std::abort();
        // static_assert(false, "missing expr type in visitor.");
      }
    }, insert_into.values.at(id));
  }
}

} // namespace lmadb::storage::column
