#include "parse_statement.hpp"

#include <gtest/gtest.h>

namespace ast = lmadb::ast;

TEST(SqlStatement, CreateTableOneColumn) {
  const auto uut = lmadb::parse_statement("create table foo (id int8);");
  ast::sql_statement expected = ast::create_table{"foo", {
    {"id", ast::type::int64}
  }};

  ASSERT_EQ(uut.value(), expected);
}

TEST(SqlStatement, CreateTableTwoColumns) {
  const auto uut = lmadb::parse_statement("create table foo (id int8, a int8);");
  ast::sql_statement expected = ast::create_table{"foo", {
    {"id", ast::type::int64},
    {"a", ast::type::int64}
  }};

  ASSERT_EQ(uut.value(), expected);
}
