#ifndef LMADB_LMADB_H
#define LMADB_LMADB_H

// Public C API for LMADB.

#ifdef __cplusplus
extern "C" {
#endif

// Return codes for API functions.
typedef enum {
  LMADB_OK,
  LMADB_ERROR
} lmadb_rc;

// The lmadb connection object.
typedef struct lmadb_connection lmadb_connection;

// Open a connection to a new or existing LMADB database.
//  path should point to the location of the database, if the path does not
//   exist, a new database is created at that location. Will error if the path
//   does exist but it doesn't appear to be an LMADB database.
//  conn allocates a new lmadb_connection object and returns it's address.
lmadb_rc lmadb_open(const char *path, lmadb_connection **conn);

// Closes an open connection. Cleans up the resources.
lmadb_rc lmadb_close(lmadb_connection *conn);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // LMADB_LMADB_H
