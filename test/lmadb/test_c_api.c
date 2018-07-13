#include "lmadb/lmadb.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

#define CHECK_API_CALL_RC(fn, expected)                          \
  rc = fn;                                                       \
  if (rc != expected) {                                          \
    printf("%d: api call " #fn " returned %d.\n", __LINE__, rc); \
    pass = false;                                                \
    goto cleanup;                                                \
  }

#define CHECK_API_CALL(fn) CHECK_API_CALL_RC(fn, LMADB_OK)

static bool is_directory(const char *path)
{
  struct stat sb;
  return stat(path, &sb) == 0 && S_ISDIR(sb.st_mode);
}

int main()
{
  bool pass = true;

  lmadb_rc rc;
  lmadb_connection *conn = NULL;
  lmadb_stmt *stmt = NULL;

  const char *sql = NULL;

  // first of all check we can safely close an objects that don't exist.
  CHECK_API_CALL(lmadb_close(conn));
  CHECK_API_CALL(lmadb_finalize(stmt));

  // create a temporary directory to store our test db in.
  char template[] = "/tmp/lmadb-XXXXXX";
  const char *dir = mkdtemp(template);
  CHECK(dir != NULL);

  char path[sizeof(template) + 6];
  {
    int written = snprintf(path, sizeof(path), "%s/lmadb", dir);
    CHECK(written >= 0 && written < (int) sizeof(path))
  }

  // check we can open a new database.
  CHECK_API_CALL(lmadb_open(path, &conn));
  CHECK(conn != NULL);

  // make sure we have created our database.
  CHECK(is_directory(path));

  // move to that directory for ease of testing that files exist.
  printf("test database: %s\n", path);
  CHECK(chdir(path) == 0);

  // close and re-open the connection, making sure it didn't disappear.
  CHECK_API_CALL(lmadb_close(conn));
  CHECK(is_directory(path));
  CHECK_API_CALL(lmadb_open(path, &conn));

  // attempt to create a statement with invalid sql.
  sql = "hello world;";
  CHECK_API_CALL_RC(lmadb_prepare(conn, sql, strlen(sql), &stmt), LMADB_ERROR);
  CHECK(stmt == NULL);

  // create a table in our database.
  sql = "create table foo (id int8);";
  CHECK_API_CALL(lmadb_prepare(conn, sql, strlen(sql), &stmt));
  CHECK(stmt != NULL);

  // now execute (but change the query string first to check it has been copied)!
  sql = NULL;
  CHECK_API_CALL_RC(lmadb_step(stmt), LMADB_DONE);

  // check our newly created table exists.
  // TODO: replace this test with "list tables/columns" commands when they exist,
  // including better tests (ie. checking the metadata content is correct).
  CHECK(access("db.tables", F_OK) != -1);
  CHECK(access("foo.columns", F_OK) != -1);

  // check we can't create a table with the same name.
  {
    sql = "create table foo (a int8);";
    CHECK_API_CALL(lmadb_prepare(conn, sql, strlen(sql), &stmt));
    CHECK(stmt != NULL);
    CHECK_API_CALL_RC(lmadb_step(stmt), LMADB_ERROR);
  }

  // check we can't create a table with two columns that have the same name.
  {
    sql = "create table foo (id int8, id int8);";
    CHECK_API_CALL(lmadb_prepare(conn, sql, strlen(sql), &stmt));
    CHECK(stmt != NULL);
    CHECK_API_CALL_RC(lmadb_step(stmt), LMADB_ERROR);
  }

cleanup:
  CHECK_API_CALL(lmadb_finalize(stmt));
  CHECK_API_CALL(lmadb_close(conn));

  CHECK(rmdir(dir) == -1);
  return pass ? 0 : 1;
}
