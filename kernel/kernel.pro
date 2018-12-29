TEMPLATE = lib
CONFIG += staticlib c++11

SOURCES += \
    init.c \
    errno.c \
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
    signal.c \
    resource.c \
    random.c \
    prctl.c \
    eventfd.c \
    fs.c \
    fs_info.c \
    poll.c \
    epoll.c

HEADERS += \
    calls.h \
    elf.h \
    errno.h \
    fs.h \
    futex.h \
    init.h \
    random.h \
    resource.h \
    signal.h \
    task.h \
    time.h \
    vdso.h

INCLUDEPATH += \
    ..
