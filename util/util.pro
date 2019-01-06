TEMPLATE = lib
CONFIG += staticlib c++11 c11

SOURCES += \
    sync.c \
    timer.c \
    win32-mman.c

HEADERS += \
    bits.h \
    list.h \
    misc.h \
    sync.h \
    timer.h \
    msvc-stdatomic.h \
    msvc-pthread.h \
    debug.h \
    win32-mman.h \
    win32-unistd.h

INCLUDEPATH += \
    ..
