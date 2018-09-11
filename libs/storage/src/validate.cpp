#include "storage/column/validate.hpp"

#include "ast/ast.hpp"

#include "meta/table_desc.hpp"

#include <iostream>

namespace lmadb::storage::column {

namespace {

// reverse mapping from c++ type to sql type. TODO: this should probably live
// somewhere else.
template <typename T>
auto type_str() -> std::string_view
{
  if (std::is_same_v<T, std::int64_t>) {
    return "int8";
  } else if (std::is_same_v<T, bool>) {
    return "boolean";
  } else {
    return "unknown";
  }
}

// given an ast::expr, determine if it can be resolved to the correct c++ type.
template <typename T>
auto check_expr_type(const ast::expr &expr, const std::size_t index) -> void
{
  std::visit([&](const auto &x) {
    using type = std::decay_t<decltype(x)>;
    if constexpr(std::is_same<type, ast::literal_value>{}) {
      std::visit([&](const auto &lit_value) {
        using lit_value_type = std::decay_t<decltype(lit_value)>;

        // for now only support the exact type, in future we can add common types.
        if (!std::is_same_v<T, lit_value_type>) {
          throw invalid_insert_into{fmt::format("unexpected type at index {}, expected {}.", index, type_str<T>())};
        }
      }, x);
    } else {
      // TODO: why won't this static assert work?
      std::cout << "got type: " << typeid(type).name() << std::endl;
      std::abort();
      // static_assert(false, "missing expr type in visitor.");
    }
  }, expr);
}

} // namespace

auto validate(const ast::insert_into &insert_into,
              const meta::table_desc &table_desc)
  -> void
{
  // check that all of the columns have a value to be inserted.
  // TODO: add support for non-null columns and the named tuple.
  if (insert_into.values.size() != table_desc.columns.size()) {
    auto msg{fmt::format("expected {} values, got {}.",
                         insert_into.values.size(),
                         table_desc.columns.size())};
    throw invalid_insert_into{std::move(msg)};
  }

  // check each value is the correct type. TODO: zip iterator.
  for (auto i{0ul}; i < insert_into.values.size(); ++i) {
    switch(table_desc.columns[i].type) {
      case ast::type::bool_:
        check_expr_type<bool>(insert_into.values[i], i);
        break;
      case ast::type::int64:
        check_expr_type<std::int64_t>(insert_into.values[i], i);
        break;
    }
  }
}

} // namespace lmadb::storage::column
