#ifndef DATABASEWRAPPER_H
#define DATABASEWRAPPER_H

#ifdef __cplusplus
class qtsqlquery
{
public:
    qtsqlquery();
};
#else
  typedef struct qtsqlquery qtsqlquery;
#endif

#ifdef __cplusplus
class qtsqldb
{
public:
    qtsqldb();
};
#else
  typedef struct qtsqldb qtsqldb;
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif // DATABASEWRAPPER_H
