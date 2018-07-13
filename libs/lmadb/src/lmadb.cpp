#include "lmadb/lmadb.h"

#include "connection.hpp"
#include "statement.hpp"

auto lmadb_open(const char *path, lmadb_connection **conn) -> lmadb_rc
{
  // connection constructor must not throw unless a try/catch is added.
  // TODO: statically check this remains true.
  if (auto *connection = new(std::nothrow) lmadb::connection{path}) {
    *conn = reinterpret_cast<lmadb_connection *>(connection);
    return LMADB_OK;    
  } else {
    return LMADB_ERROR;
  }
}

auto lmadb_close(lmadb_connection *conn) -> lmadb_rc
{
  delete reinterpret_cast<lmadb::connection *>(conn);
  return LMADB_OK;
}

auto lmadb_prepare(lmadb_connection *conn, const char *sql, int length, lmadb_stmt **stmt)
  -> lmadb_rc
{
  if (!conn || !sql) {
    return LMADB_ERROR;
  }

  try {
    auto &connection{*reinterpret_cast<lmadb::connection *>(conn)};
    auto statement = connection.create_statement({sql, static_cast<std::size_t>(length)});

    *stmt = reinterpret_cast<lmadb_stmt *>(statement.release());
    return LMADB_OK;
  } catch([[maybe_unused]] const std::exception &what) {
    // TODO: use our own exception types.
    return LMADB_ERROR;
  }
}

auto lmadb_finalize(lmadb_stmt *stmt) -> lmadb_rc
{
  delete reinterpret_cast<lmadb::statement *>(stmt);
  return LMADB_OK;
}

auto lmadb_step(lmadb_stmt *stmt) -> lmadb_rc
{
  if (!stmt) {
    return LMADB_ERROR;
  }

  try {
    using lmadb::step_status;

    auto &statement{*reinterpret_cast<lmadb::statement *>(stmt)};
    switch(statement.step()) {
      case step_status::row: return LMADB_ROW;
      case step_status::done: return LMADB_DONE;
    }
  } catch([[maybe_unused]] const std::exception &what) {
    // TODO: use our own exception types.
    return LMADB_ERROR;
  }
}
