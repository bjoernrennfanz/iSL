#include "qtdbwrapper.h"

int qtsql_errcode(qtsqldb *db)
{
    return 0;
}

const char *qtsql_errmsg(qtsqldb *db)
{
    return Q_NULLPTR;
}

int qtsql_exec(qtsqldb *db, const char *zSql, int (*callback)(void*,int,char**,char**), void *, char **pzErrMsg)
{

}

int qtsql_finalize(qtsqlquery *pStmt)
{

}

int qtsql_prepare(qtsqldb *db, const char *zSql, size_t nByte, qtsqlquery **ppStmt, const char **pzTails)
{

}

int qtsql_reset(qtsqlquery *pStmt)
{

}

int qtsql_step(qtsqlquery *pStmt)
{

}

int qtsql_bind_blob(qtsqlquery *pStmt, int iCol, const void *zData, size_t nData, void(*qtsql_deleter)(void*))
{

}

int qtsql_bind_int64(qtsqlquery *pStmt, int iCol, int64_t value)
{

}

const void *qtsql_column_blob(qtsqlquery *pStmt, int iCol)
{

}

int qtsql_column_bytes(qtsqlquery *pStmt, int iCol)
{

}

int qtsql_column_int(qtsqlquery *pStmt, int iCol)
{

}

int64_t qtsql_column_int64(qtsqlquery *pStmt, int iCol)
{

}

const unsigned char *qtsql_column_text(qtsqlquery *pStmt, int iCol)
{

}
