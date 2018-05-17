#include "lmadb/lmadb.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#ifdef __cplusplus
#error "Test should be compiled as C."
#endif

#define CHECK(pred)                                              \
  if (!(pred)) {                                                 \
    printf("%d: predicate \"" #pred "\" not true.\n", __LINE__); \
    pass = false;                                                \
    goto cleanup;                                                \
  }

#define CHECK_API_CALL(fn)                                       \
  rc = fn;                                                       \
  if (rc != LMADB_OK) {                                          \
    printf("%d: api call " #fn " returned %d.\n", __LINE__, rc); \
    pass = false;                                                \
    goto cleanup;                                                \
  }

static bool is_directory(const char *path)
{
  struct stat sb;
  return stat(path, &sb) == 0 && S_ISDIR(sb.st_mode);
}

int main()
{
  bool pass = true;

  lmadb_connection *conn = NULL;
  lmadb_rc rc;

  // first of all check we can safely close an unopened connection.
  CHECK_API_CALL(lmadb_close(conn));

  // create a temporary directory to store our test db in.
  char template[] = "/tmp/lmadb-XXXXXX";
  const char *dir = mkdtemp(template);
  CHECK(dir != NULL);

  char path[sizeof(template) + 6];
  snprintf(path, sizeof(path), "%s/lmadb", dir);

  // check we can open a new database.
  CHECK_API_CALL(lmadb_open(path, &conn));
  CHECK(conn != NULL);

  // make sure we have created our database.
  CHECK(is_directory(path));

  // close and re-open the connection, making sure it didn't disappear.
  CHECK_API_CALL(lmadb_close(conn));
  CHECK(is_directory(path));
  CHECK_API_CALL(lmadb_open(path, &conn));

cleanup:
  CHECK_API_CALL(lmadb_close(conn));
  CHECK(rmdir(dir) == -1);
  return pass ? 0 : 1;
}
