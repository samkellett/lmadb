#include "ast/parse_statement.hpp"

#include <gtest/gtest.h>

namespace ast = lmadb::ast;

TEST(SqlStatement, CreateTableOneColumn) {
  const auto uut = lmadb::ast::parse_statement("create table foo (id int8)");
  ast::sql_statement expected = ast::create_table{"foo", {
    {"id", ast::type::int64}
  }};

  ASSERT_EQ(uut.value(), expected);
}

TEST(SqlStatement, CreateTableTwoColumns) {
  const auto uut = lmadb::ast::parse_statement(R"sql(
    create table foo (
      id int8,
      a boolean
    ))sql");
  ast::sql_statement expected = ast::create_table{"foo", {
    {"id", ast::type::int64},
    {"a", ast::type::bool_}
  }};

  ASSERT_EQ(uut.value(), expected);
}

TEST(SqlStatement, InsertIntoInt64) {
  const auto uut = lmadb::ast::parse_statement("insert into foo values (1, 2, 3)");
  ast::sql_statement expected = ast::insert_into{"foo", {1ll, 2ll, 3ll}};

  ASSERT_EQ(uut.value(), expected);
}

TEST(SqlStatement, InsertIntoBool) {
  const auto uut = lmadb::ast::parse_statement("insert into foo values (false, true)");
  ast::sql_statement expected = ast::insert_into{"foo", {false, true}};

  ASSERT_EQ(uut.value(), expected);
}

TEST(SqlStatement, InsertIntoMix) {
  const auto uut = lmadb::ast::parse_statement("insert into foo values (1, true)");
  ast::sql_statement expected = ast::insert_into{"foo", {1ll, true}};

  ASSERT_EQ(uut.value(), expected);
}

TEST(SqlStatement, SelectColumns) {
  const auto uut = lmadb::ast::parse_statement("select a, b from foo");
  ast::sql_statement expected = ast::select{{"a", "b"}, "foo"};

  ASSERT_EQ(uut.value(), expected);
}
