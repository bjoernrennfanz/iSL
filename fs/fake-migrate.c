#include "kernel/fs.h"
#include "util/debug.h"
#include "kernel/user-errno.h"

// The value of the user_version pragma is used to decide what needs migrating.

static struct migration {
    const char *sql;
    void (*migrate)(struct mount *mount);
} migrations[] = {
    // version 0 to version 1
    {
        "create index inode_to_path on paths (inode, path);"
    },
};

int fakefs_migrate(struct mount *mount) {
    int err;
#define CHECK_ERR() \
    if (err != QTSQL_OK && err != QTSQL_ROW && err != QTSQL_DONE) \
        die("sqlite error while migrating: %s\n", qtsql_errmsg(mount->db));
#define EXEC(sql) \
    err = qtsql_exec(mount->db, sql, NULL, NULL, NULL); \
    CHECK_ERR();
#define PREPARE(sql) ({ \
    qtsqlquery *stmt; \
    err = qtsql_prepare(mount->db, sql, -1, &stmt, NULL); \
    CHECK_ERR(); \
    stmt; \
})
#define STEP(stmt) ({ \
    err = qtsql_step(stmt); \
    CHECK_ERR(); \
    err == QTSQL_ROW; \
})
#define RESET(stmt) \
    err = qtsql_reset(stmt); \
    CHECK_ERR()
#define FINALIZE(stmt) \
    err = qtsql_finalize(stmt); \
    CHECK_ERR()

    qtsqlquery *user_version = PREPARE("pragma user_version");
    STEP(user_version);
    int version = qtsql_column_int(user_version, 0);
    FINALIZE(user_version);

    EXEC("begin");
    int versions = sizeof(migrations)/sizeof(migrations[0]);
    while (version < versions) {
        struct migration m = migrations[version];
        if (m.sql != NULL)
            EXEC(m.sql);
        if (m.migrate != NULL)
            m.migrate(mount);
        version++;
    }
    // for some reason placeholders aren't allowed in pragmas
    char pragma_user_version_sql[30];
    sprintf(&pragma_user_version_sql[0], "pragma user_version = %d", version);
    EXEC(&pragma_user_version_sql[0]);

    EXEC("commit");

    return 0;
}
