QT       += concurrent

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

DISTFILES += \
    $$PWD/LICENSE \
    $$PWD/README.md \
    $$PWD/xdataconvertorwidget.cmake

FORMS += \
    $$PWD/dialogxdataconvertor.ui \
    $$PWD/xdataconvertorwidget.ui

HEADERS += \
    $$PWD/dialogxdataconvertor.h \
    $$PWD/xdataconvertorwidget.h

SOURCES += \
    $$PWD/dialogxdataconvertor.cpp \
    $$PWD/xdataconvertorwidget.cpp
