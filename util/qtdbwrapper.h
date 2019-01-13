#ifndef DATABASEWRAPPER_H
#define DATABASEWRAPPER_H

#include <stdint.h>

#define QTSQL_OK    0
#define QTSQL_ERR   1
#define QTSQL_ROW   100
#define QTSQL_DONE  101

typedef void (*qtsql_destructor_type)(void*);
#define QTSQL_STATIC      ((qtsql_destructor_type)0)
#define QTSQL_TRANSIENT   ((qtsql_destructor_type)-1)

#ifdef __cplusplus
#include <QSqlQuery>
typedef QSqlQuery qtsqlquery;
#else
  typedef struct qtsqlquery qtsqlquery;
#endif

#ifdef __cplusplus
#include <QSqlDatabase>
typedef QSqlDatabase qtsqldb;
#else
typedef struct qtsqldb qtsqldb;
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern int qtsql_errcode(qtsqldb *db);
extern const char *qtsql_errmsg(qtsqldb *db);

extern int qtsql_exec(qtsqldb *db, const char *zSql, int (*callback)(void*,int,char**,char**), void *, char **pzErrMsg);
extern int qtsql_finalize(qtsqlquery *pStmt);
extern int qtsql_prepare(qtsqldb *db, const char *zSql, size_t nByte, qtsqlquery **ppStmt, const char **pzTails);
extern int qtsql_reset(qtsqlquery *pStmt);
extern int qtsql_step(qtsqlquery *pStmt);

extern int qtsql_bind_blob(qtsqlquery *pStmt, int iCol, const void *zData, size_t nData, void(*qtsql_deleter)(void*));
extern int qtsql_bind_int64(qtsqlquery *pStmt, int iCol, int64_t value);

extern const void *qtsql_column_blob(qtsqlquery *pStmt, int iCol);
extern int qtsql_column_bytes(qtsqlquery *pStmt, int iCol);
extern int qtsql_column_int(qtsqlquery *pStmt, int iCol);
extern int64_t qtsql_column_int64(qtsqlquery *pStmt, int iCol);
extern const unsigned char *qtsql_column_text(qtsqlquery *pStmt, int iCol);

#ifdef __cplusplus
}
#endif

#endif // DATABASEWRAPPER_H
