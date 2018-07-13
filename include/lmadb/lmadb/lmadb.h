#ifndef LMADB_LMADB_H
#define LMADB_LMADB_H

// Public C API for LMADB, based off of the sqlite3 C interface.

#ifdef __cplusplus
extern "C" {
#endif

// Return codes for API functions.
typedef enum {
  LMADB_OK,
  LMADB_ERROR,
  LMADB_DONE,
  LMADB_ROW
} lmadb_rc;

// The lmadb connection object.
typedef struct lmadb_connection lmadb_connection;

// A compiled lmadb statement.
typedef struct lmadb_stmt lmadb_stmt;

// Open a connection to a new or existing LMADB database.
//  path should point to the location of the database, if the path does not
//   exist, a new database is created at that location. Will error if the path
//   does exist but it doesn't appear to be an LMADB database.
//  conn allocates a new lmadb_connection object and returns it's address.
lmadb_rc lmadb_open(const char *path, lmadb_connection **conn);

// Closes an open connection. Cleans up the resources.
lmadb_rc lmadb_close(lmadb_connection *conn);

// Compile a SQL statement into a lmadb_stmt. The SQL statement does not have
// to be zero-terminated. Currently any extra characters are ignored.
lmadb_rc lmadb_prepare(lmadb_connection *conn, const char *sql, int length, lmadb_stmt **stmt);

// Deletes a previously created statement.
lmadb_rc lmadb_finalize(lmadb_stmt *stmt);

// Evaluate a prepared statement. This can be executed multiple times if there
// are multiple results. Once LMADB_DONE is returned the statement is finished.
// Calling this function after a statement is finished is an error. If data is
// returned then this function will return LMADB_ROW.
lmadb_rc lmadb_step(lmadb_stmt *stmt);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // LMADB_LMADB_H
