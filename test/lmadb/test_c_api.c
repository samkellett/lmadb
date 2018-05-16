#include "lmadb/lmadb.h"

#include <stdio.h>

#define CHECK(pred)                                            \
  if (!(pred)) {                                               \
    printf("%d: predicate \"" #pred "\" not true.", __LINE__); \
    return 1;                                                  \
  }

#define CHECK_API_CALL(fn)                                     \
  rc = fn;                                                     \
  if (rc != LMADB_OK) {                                        \
    printf("%d: api call " #fn " returned %d.", __LINE__, rc); \
    return 1;                                                  \
  }

int main()
{
  lmadb_rc rc;

  lmadb_connection *conn = NULL;
  CHECK_API_CALL(lmadb_open("/path/to/db", &conn));
  CHECK(conn != NULL);

  CHECK_API_CALL(lmadb_close(conn));
  return 0;
}
