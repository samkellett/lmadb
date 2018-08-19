#include "lmadb/lmadb.h"

#include <pybind11/pybind11.h>

namespace py = pybind11;

namespace {

template <typename T, auto Constructor, auto Destructor>
class wrapper {
public:
  template <typename ...Args>
  wrapper(Args&& ...args)
  : ptr_{nullptr}
  {
    // TODO: error checking.
    T *ptr;
    Constructor(std::forward<Args>(args)..., &ptr);
    ptr_ = std::shared_ptr<T>{ptr, Destructor};
  }

  operator T *() const {
    return ptr_.get();
  }

private:
  std::shared_ptr<T> ptr_;
};

using connection_wrapper = wrapper<::lmadb_connection, ::lmadb_open, ::lmadb_close>;
using statement_wrapper = wrapper<::lmadb_stmt, ::lmadb_prepare, ::lmadb_finalize>;

} // unnamed namespace

PYBIND11_MODULE(pylmadb, m) {
  m.attr("__version__") = "dev";
  m.doc() = "Python bindings for LMADB.";

  py::class_<connection_wrapper>(m, "connection")
    .def(py::init<const char *>())
    .def("prepare", [](const connection_wrapper &conn, const char *sql) {
      return statement_wrapper{conn, sql, std::strlen(sql)};
    })
    .def("list_tables", [](const connection_wrapper &conn) {
      ::lmadb_table_list *x;
      ::lmadb_list_tables(conn, &x);
      return std::shared_ptr<::lmadb_table_list>{x, ::lmadb_free_table_list};
    })
    .def("describe_table", [](const connection_wrapper &conn, const char *table) {
      ::lmadb_table_desc *x;
      ::lmadb_describe_table(conn, table, &x);
      return std::shared_ptr<::lmadb_table_desc>{x, ::lmadb_free_table_desc};
    })
    ;

  py::class_<statement_wrapper>(m, "stmt")
    .def("__iter__", [](const statement_wrapper &stmt) { return stmt; })
    .def("__next__", [](const statement_wrapper &stmt) {
      auto rc = ::lmadb_step(stmt);
      switch(rc) {
        case LMADB_DONE:
          throw pybind11::stop_iteration{};
          break;
        case LMADB_ROW:
          // TODO: return values.
          break;
        case LMADB_OK:
          // should never see this from step.
          assert(false);
          break;
        case LMADB_ERROR:
          // TODO: provide an appropriate error string here.
          throw std::runtime_error{"internal error"};
      }
    })
    ;

  py::class_<::lmadb_table_list, std::shared_ptr<::lmadb_table_list>>(m, "table_list")
    .def("__getitem__", [](const ::lmadb_table_list &tables, std::size_t idx) {
      if (idx >= tables.size) {
        throw pybind11::index_error{};
      }

      return tables.names[idx];
    })
    ;

  py::class_<::lmadb_table_desc, std::shared_ptr<::lmadb_table_desc>>(m, "table_desc")
    .def("__getitem__", [](const ::lmadb_table_desc &desc, std::size_t idx) {
      if (idx >= desc.size) {
        throw pybind11::index_error{};
      }

      return std::make_pair(desc.columns[idx], desc.types[idx]);
    })
    ;
}
