#include "cxx/adt/persistent_flat_map.hpp"

#include "testlib/tmp_dir_fixture.hpp"

#include <gtest/gtest.h>

namespace cxx = lmadb::cxx;

using lmadb::testlib::TmpDirFixture;

TEST_F(TmpDirFixture, CreateFlatMap)
{
  const auto file = tmpdir / "badger.tbl";

  {
    cxx::persistent_flat_map<int, float> uut{file};
    ASSERT_EQ(0, uut.size());

    auto result = uut.insert({42, 0.123f});
    ASSERT_EQ(*result.first, std::make_pair(42, 0.123f));
    ASSERT_TRUE(result.second);

    result = uut.insert({42, 0.123f});
    ASSERT_FALSE(result.second);
  }

  {
    cxx::persistent_flat_map<int, float> uut{file};
    ASSERT_EQ(1, uut.size());
  }
}
