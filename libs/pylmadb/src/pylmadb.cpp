#include "lmadb/lmadb.h"

#include <pybind11/pybind11.h>

namespace py = pybind11;

namespace {

class connection_wrapper {
public:
  template <typename ...Args>
  connection_wrapper(Args&& ...args)
  : connection_{nullptr}
  {
    // TODO: error checking.
    ::lmadb_connection *ptr;
    ::lmadb_open(std::forward<Args>(args)..., &ptr);
    connection_ = std::shared_ptr<::lmadb_connection>{ptr, ::lmadb_close};
  }

private:
  std::shared_ptr<::lmadb_connection> connection_;
};

} // unnamed namespace

PYBIND11_MODULE(pylmadb, m) {
  m.attr("__version__") = "dev";
  m.doc() = "Python bindings for LMADB.";

  py::class_<connection_wrapper>(m, "connection")
    .def(py::init<const char *>())
    ;
}
