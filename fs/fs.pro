TEMPLATE = lib
CONFIG += staticlib c++11

SOURCES += \
    mount.c \
    fd.c \
    stat.c \
    dir.c \
    generic.c \
    path.c \
    real.c \
    fake.c \
    fake-rebuild.c \
    fake-migrate.c \
    proc.c \
    proc/entry.c \
    proc/root.c \
    adhoc.c \
    sock.c \
    pipe.c \
    dev.c \
    mem.c \
    tty.c \
    tty-real.c \
    poll.c

HEADERS += \
    dev.h \
    fd.h \
    mem.h \
    path.h \
    poll.h \
    proc.h \
    sock.h \
    stat.h \
    tty.h

INCLUDEPATH += \
    ..
