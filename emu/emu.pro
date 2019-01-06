TEMPLATE = lib
CONFIG += staticlib c++11

SOURCES += \
    memory.c \
    tlb.c \
    interp.c \
    uint128.c \
    float80.c \
    fpu.c

HEADERS += \
    cpu.h \
    cpuid.h \
    decode.h \
    float80.h \
    fpu.h \
    interp/fpu.h \
    interp/sse.h \
    interrupt.h \
    memory.h \
    modrm.h \
    regid.h \
    tlb.h \
    uint128.h

INCLUDEPATH += \
    ..
