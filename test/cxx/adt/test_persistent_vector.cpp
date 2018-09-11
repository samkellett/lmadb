#include "cxx/adt/persistent_vector.hpp"

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

TEST_F(Fixture, CreateVector)
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
