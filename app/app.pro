# QMake project file for iSL application
TEMPLATE = app
TARGET = iSL

QT += qml quick core
CONFIG += c++11

SOURCES += \
    main.cpp \
    islappui.cpp \

HEADERS += \
    islappui.h

DISTFILES += \
    ../qml/main.qml

RESOURCES += \
    ../qml.qrc

INCLUDEPATH += \
    ..
