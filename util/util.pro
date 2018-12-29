TEMPLATE = lib
CONFIG += staticlib c++11 c11

SOURCES += \
    sync.c \
    timer.c

HEADERS += \
    bits.h \
    list.h \
    sync.h \
    timer.h

INCLUDEPATH += \
    ..
