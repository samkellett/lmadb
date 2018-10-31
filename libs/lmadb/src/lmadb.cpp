#include "lmadb/lmadb.h"

#include "connection.hpp"
#include "statement.hpp"

#include <boost/range/algorithm/copy.hpp>

auto lmadb_open(const char *path, lmadb_connection **conn) -> lmadb_rc
{
  if (!path || !conn) {
    return LMADB_ERROR;
  }

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
  if (!conn || !sql || !stmt) {
    // TODO: use set_error when possible here.
    return LMADB_ERROR;
  }

  auto &connection{*reinterpret_cast<lmadb::connection *>(conn)};

  try {
    auto statement = connection.create_statement({sql, static_cast<std::size_t>(length)});

    *stmt = reinterpret_cast<lmadb_stmt *>(statement.release());
    return LMADB_OK;
  } catch(const std::exception &e) {
    // TODO: use our own exception types.
    connection.set_error(e.what());
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

  auto &statement{*reinterpret_cast<lmadb::statement *>(stmt)};
  try {
    // if the statement iterator is finished then we haven't begun yet. because of this 
    // it is invalid to call lmadb_step with an aleady exhausted coroutine.
    const auto end{std::end(statement.row)};
    if (statement.it == end) {
      // begin the coroutine.
      statement.it = std::begin(statement.row);
    }

    // continue until we get a value back.
    // TODO: make our own coroutine type that means we don't have to do this.
    while (statement.it != end && *statement.it == nullptr) {
      ++statement.it;
    }

    // have we finished now?
    if (statement.it == end) {
      return LMADB_DONE;
    } else {
      return LMADB_ROW;
    }
  } catch(const std::exception &e) {
    // TODO: use our own exception types.
    statement.error = e.what();
    return LMADB_ERROR;
  }
}

auto lmadb_errmsg(lmadb_connection *conn) -> const char *
{
  auto &connection{*reinterpret_cast<lmadb::connection *>(conn)};
  return connection.error().c_str();
}

auto lmadb_errstr(lmadb_rc err) -> const char *
{
  switch(err) {
    case LMADB_OK: return "LMADB_OK";
    case LMADB_ERROR: return "LMADB_ERROR";
    case LMADB_DONE: return "LMADB_DONE";
    case LMADB_ROW: return "LMADB_ROW";
  }
}

auto lmadb_list_tables(lmadb_connection *conn, lmadb_table_list **table_list)
  -> lmadb_rc
{
  if (!conn || !table_list) {
    return LMADB_ERROR;
  }

  auto &connection{*reinterpret_cast<lmadb::connection *>(conn)};
  const auto tables = connection.list_tables();

  // No exception/memory safety here... but this is a tempoary method.
  char **name_array = new char*[tables.size()];
  *table_list = new lmadb_table_list{name_array, tables.size()};

  for (std::size_t i = 0; i < tables.size(); ++i) {
    char *name = new char[tables[i].size()+1];
    boost::copy(tables[i], name);

    (*table_list)->names[i] = name;
  }

  return LMADB_OK;
}

auto lmadb_describe_table(lmadb_connection *conn, const char *table, lmadb_table_desc **desc)
  -> lmadb_rc
{
  if (!conn || !table || !desc) {
    return LMADB_ERROR;
  }

  auto &connection{*reinterpret_cast<lmadb::connection *>(conn)};
  const auto table_desc = connection.describe_table(table);

  // No exception/memory safety here... but this is a tempoary method.
  char **column_array = new char*[table_desc.size()];
  char **type_array = new char*[table_desc.size()];
  *desc = new lmadb_table_desc{column_array, type_array, table_desc.size()};

  for (std::size_t i = 0; i < table_desc.size(); ++i) {
    char *column = new char[table_desc[i].first.size()+1];
    char *type = new char[table_desc[i].second.size()+1];

    boost::copy(table_desc[i].first, column);
    boost::copy(table_desc[i].second, type);

    (*desc)->columns[i] = column;
    (*desc)->types[i] = type;
  }

  return LMADB_OK;
}

auto lmadb_free_table_list(lmadb_table_list *tables) -> lmadb_rc
{
  if (tables) {
    for (std::size_t i = 0; i < tables->size; ++i) {
      delete[] tables->names[i];
    }

    delete tables;
  }

  return LMADB_OK;
}

auto lmadb_free_table_desc(lmadb_table_desc *desc) -> lmadb_rc
{
  if (desc) {
    for(std::size_t i = 0; i < desc->size; ++i) {
      delete[] desc->columns[i];
      delete[] desc->types[i];
    }

    delete desc;
  }

  return LMADB_OK;
}
