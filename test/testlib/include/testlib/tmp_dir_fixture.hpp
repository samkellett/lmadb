#include "cxx/filesystem.hpp"

#include <gtest/gtest.h>

namespace lmadb::testlib {

struct TmpDirFixture : public ::testing::Test {
  cxx::filesystem::path tmpdir{cxx::filesystem::unique_path()};

  TmpDirFixture() {
    cxx::filesystem::create_directories(tmpdir);
    std::cout << "tmpdir = " << tmpdir << std::endl;
  }

  ~TmpDirFixture() {
    cxx::filesystem::remove_all(tmpdir);
  }
};

} // namespace lmadb::testlib
