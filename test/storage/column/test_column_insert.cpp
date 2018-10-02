#include "storage/column/insert.hpp"

#include "ast/ast.hpp"

#include "meta/table_desc.hpp"

#include "storage/column/column_vector.hpp"

#include "testlib/tmp_dir_fixture.hpp"

#include <gmock/gmock.h>

namespace ast = lmadb::ast;
namespace column = lmadb::storage::column;
namespace meta = lmadb::meta;

using lmadb::testlib::TmpDirFixture;

using ::testing::ElementsAre;

TEST_F(TmpDirFixture, Insert)
{
  {
    meta::table_desc desc{
      {17},
      meta::column_metadata_vector{tmpdir / "columns"}
    };

    desc.columns.push_back({"a", ast::type::int64});
    desc.columns.push_back({"b", ast::type::bool_});

    ast::insert_into insert{"foo", {42ll, true}};

    column::insert(tmpdir, ast::insert_into{"foo", {42ll, true}}, desc);
    column::insert(tmpdir, ast::insert_into{"foo", {78ll, false}}, desc);
  }

  {
    column::column_vector<std::int64_t> column_a{tmpdir / "17.0.data"};
    ASSERT_EQ(column_a.size(), 2);
    ASSERT_THAT(column_a, ElementsAre(42ll, 78ll));
  }

  {
    column::column_vector<std::int64_t> column_b{tmpdir / "17.1.data"};
    ASSERT_EQ(column_b.size(), 2);
    ASSERT_THAT(column_b, ElementsAre(true, false));
  }
}
