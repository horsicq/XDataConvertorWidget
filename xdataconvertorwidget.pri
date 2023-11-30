QT       += concurrent

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/dialogxdataconvertor.h \
    $$PWD/xdataconvertorwidget.h

SOURCES += \
    $$PWD/dialogxdataconvertor.cpp \
    $$PWD/xdataconvertorwidget.cpp

FORMS += \
    $$PWD/dialogxdataconvertor.ui \
    $$PWD/xdataconvertorwidget.ui

DISTFILES += \
    $$PWD/LICENSE \
    $$PWD/README.md \
    $$PWD/xdataconvertorwidget.cmake
