#include "lmadb/lmadb.h"

#include <pybind11/pybind11.h>

namespace {

namespace py = pybind11;

class connection_wrapper {
public:
  connection_wrapper(const char *path)
  : conn_{nullptr}
  {
    ::lmadb_connection *conn;
    const auto rc{::lmadb_open(path, &conn)};
    switch(rc) {
      case LMADB_OK: break;
      case LMADB_ERROR:
        assert(!conn && "Connection must still be NULL or it will leak.");
        throw std::runtime_error{"Unable to create a connection."};
      case LMADB_ROW:
      case LMADB_DONE:
        assert(false && "Unexpected return code.");
    }
    conn_ = std::shared_ptr<::lmadb_connection>{conn, ::lmadb_close};
  }

  operator ::lmadb_connection *() const {
    return conn_.get();
  }

private:
  std::shared_ptr<::lmadb_connection> conn_;
};

class statement_wrapper {
public:
  statement_wrapper(lmadb_connection *conn, const char *sql, size_t length)
  : conn_{conn},
    stmt_{nullptr}
  {
    ::lmadb_stmt *stmt;
    assert(length <= std::numeric_limits<int>::max());
    const auto rc{::lmadb_prepare(conn, sql, static_cast<int>(length), &stmt)};
    switch(rc) {
      case LMADB_OK: break;
      case LMADB_ERROR:
        throw std::runtime_error{error()};
      case LMADB_ROW:
      case LMADB_DONE:
        assert(false && "Unexpected return code.");
    }
    stmt_ = std::shared_ptr<::lmadb_stmt>{stmt, ::lmadb_finalize};
  }

  operator ::lmadb_stmt *() const {
    return stmt_.get();
  }

  auto error() const -> const char * {
    return ::lmadb_errmsg(conn_);
  }

private:
  // need to keep hold of the connection pointer to call lmadb_errmsg.
  ::lmadb_connection *conn_;
  std::shared_ptr<::lmadb_stmt> stmt_;
};

} // unnamed namespace

// TODO: explore setting a python exception without rethrowing. also use our
// own exception types.
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
          throw std::runtime_error{stmt.error()};
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
