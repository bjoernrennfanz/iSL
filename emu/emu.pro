TEMPLATE = lib
CONFIG += staticlib c++11 c11

SOURCES += \
    memory.c \
    tlb.c \
    fpu.c \
    float80.c \
    interp.c

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
    tlb.h

INCLUDEPATH += \
    ..
