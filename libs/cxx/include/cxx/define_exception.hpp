#ifndef LMADB_CXX_DEFINE_EXCEPTION_HPP
#define LMADB_CXX_DEFINE_EXCEPTION_HPP

#include <fmt/format.h>

#include <stdexcept>

namespace lmadb::cxx {

#define LMADB_DEFINE_EXCEPTION(error)                                 \
  class error : public std::runtime_error {                           \
  public:                                                             \
    template <typename ...Args>                                       \
    error(Args&& ...args)                                             \
    : std::runtime_error{fmt::format(std::forward<Args>(args)...)} {} \
  }

} // namespace lmadb::cxx

#endif // LMADB_CXX_DEFINE_EXCEPTION_HPP
