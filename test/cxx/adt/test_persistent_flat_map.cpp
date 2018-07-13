#include "cxx/adt/persistent_flat_map.hpp"

#include "cxx/filesystem.hpp"

#include <gtest/gtest.h>

namespace cxx = lmadb::cxx;

// TODO: put this into an internal test library.
struct Fixture : public ::testing::Test {
  cxx::filesystem::path tmpdir{cxx::filesystem::unique_path()};

  Fixture() {
    cxx::filesystem::create_directories(tmpdir);
    std::cout << "tmpdir = " << tmpdir << std::endl;
  }

  ~Fixture() {
    cxx::filesystem::remove_all(tmpdir);
  }
};

TEST_F(Fixture, CreateFlatMap)
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
