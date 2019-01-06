TEMPLATE = lib
CONFIG += staticlib c++11

SOURCES += \
    init.c \
    calls.c \
    user.c \
    vdso.c \
    task.c \
    group.c \
    log.c \
    fork.c \
    exec.c \
    exit.c \
    time.c \
    mmap.c \
    uname.c \
    tls.c \
    futex.c \
    getset.c \
    resource.c \
    random.c \
    prctl.c \
    eventfd.c \
    fs.c \
    fs_info.c \
    poll.c \
    epoll.c \
    user-signal.c \
    user-errno.c

HEADERS += \
    calls.h \
    elf.h \
    fs.h \
    futex.h \
    init.h \
    random.h \
    resource.h \
    task.h \
    time.h \
    vdso.h \
    user-signal.h \
    user-errno.h

INCLUDEPATH += \
    ..
