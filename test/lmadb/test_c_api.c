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

#define CHECK(pred)                                                   \
  if (!(pred)) {                                                      \
    printf("line %d: predicate \"%s\" not true.\n", __LINE__, #pred); \
    pass = false;                                                     \
    goto cleanup;                                                     \
  }

#define CHECK_API_CALL_RC(fn, expected)                               \
  rc = fn;                                                            \
  if (rc != expected) {                                               \
    printf("line %d: api call %s returned %d.\n", __LINE__, #fn, rc); \
    pass = false;                                                     \
    goto cleanup;                                                     \
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
  lmadb_table_list *tables = NULL;
  lmadb_table_desc *table_desc = NULL;

  const char *sql = NULL;

  // first of all check we can safely close an objects that don't exist.
  CHECK_API_CALL(lmadb_close(conn));
  CHECK_API_CALL(lmadb_finalize(stmt));
  CHECK_API_CALL(lmadb_free_table_list(tables));
  CHECK_API_CALL(lmadb_free_table_desc(table_desc));

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
  printf("open database\n");
  CHECK_API_CALL(lmadb_open(path, &conn));
  CHECK(conn != NULL);

  // make sure we have created our database.
  printf("test database: %s\n", path);
  CHECK(is_directory(path));

  // close and re-open the connection, making sure it didn't disappear.
  printf("close and reopen\n");
  CHECK_API_CALL(lmadb_close(conn));
  CHECK(is_directory(path));
  CHECK_API_CALL(lmadb_open(path, &conn));

  // attempt to create a statement with invalid sql.
  sql = "hello world;";
  CHECK_API_CALL_RC(lmadb_prepare(conn, sql, strlen(sql), &stmt), LMADB_ERROR);
  CHECK(stmt == NULL);
  CHECK(strcmp(lmadb_errmsg(conn), "invalid SQL: 'hello world;'.") == 0);

  // create a table in our database.
  printf("create table foo\n");
  sql = "create table foo (id int8);";
  CHECK_API_CALL(lmadb_prepare(conn, sql, strlen(sql), &stmt));
  CHECK(stmt != NULL);

  // now execute (but change the query string first to check it has been copied)!
  sql = NULL;
  CHECK_API_CALL_RC(lmadb_step(stmt), LMADB_DONE);
  CHECK_API_CALL(lmadb_finalize(stmt));
  stmt = NULL;

  // check our newly created table exists.
  printf("list tables\n");
  CHECK_API_CALL(lmadb_list_tables(conn, &tables));
  CHECK(tables != NULL);
  CHECK(tables->size == 1);
  CHECK(tables->names != NULL);
  CHECK(strcmp(tables->names[0], "foo") == 0);
  CHECK_API_CALL(lmadb_free_table_list(tables));
  tables = NULL;

  printf("describe table foo\n");
  CHECK_API_CALL(lmadb_describe_table(conn, "foo", &table_desc));
  CHECK(table_desc != NULL);
  CHECK(table_desc->size == 1);
  CHECK(table_desc->columns != NULL && table_desc->types != NULL);
  CHECK(strcmp(table_desc->columns[0], "id") == 0);
  CHECK(strcmp(table_desc->types[0], "int8") == 0);
  CHECK_API_CALL(lmadb_free_table_desc(table_desc));
  table_desc = NULL;

cleanup:
  printf("cleanup\n");
  CHECK_API_CALL(lmadb_finalize(stmt));
  CHECK_API_CALL(lmadb_close(conn));
  CHECK_API_CALL(lmadb_free_table_list(tables));
  CHECK_API_CALL(lmadb_free_table_desc(table_desc));

  CHECK(rmdir(dir) == -1);
  return pass ? 0 : 1;
}
