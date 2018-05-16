#include "lmadb/lmadb.h"

#include "connection.hpp"

lmadb_rc lmadb_open(const char *path, lmadb_connection **conn)
{
  if (auto *c = new(std::nothrow) lmadb::connection{path}) {
    *conn = reinterpret_cast<lmadb_connection *>(c);
    return LMADB_OK;    
  } else {
    return LMADB_ERROR;
  }
}

lmadb_rc lmadb_close(lmadb_connection *conn)
{
  delete reinterpret_cast<lmadb::connection *>(conn);
  return LMADB_OK;
}
