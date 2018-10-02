#include "cxx/adt/persistent_vector.hpp"

#include "testlib/tmp_dir_fixture.hpp"

#include <gtest/gtest.h>

namespace cxx = lmadb::cxx;

using lmadb::testlib::TmpDirFixture;

TEST_F(TmpDirFixture, CreateVector)
{
  const auto file = tmpdir / "badger.tbl";

  {
    cxx::persistent_vector<int> uut{file};
    ASSERT_EQ(0, uut.size());

    uut.push_back(42);
    ASSERT_EQ(1, uut.size());
    ASSERT_EQ(42, uut[0]);
  }

  {
    cxx::persistent_vector<int> uut{file};
    ASSERT_EQ(1, uut.size());
    ASSERT_EQ(42, uut[0]);
  }
}
