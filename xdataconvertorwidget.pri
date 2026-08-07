QT       += concurrent

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/dialogxdataconvertor.h \
    $$PWD/xdataconvertordecompress.h \
    $$PWD/xdataconvertorwidget.h

SOURCES += \
    $$PWD/dialogxdataconvertor.cpp \
    $$PWD/xdataconvertordecompress.cpp \
    $$PWD/xdataconvertorwidget.cpp

FORMS += \
    $$PWD/dialogxdataconvertor.ui \
    $$PWD/xdataconvertorwidget.ui

!contains(XCONFIG, xdataconvertor) {
    XCONFIG += xdataconvertor
    include($$PWD/../Formats/xdataconvertor.pri)
}

!contains(XCONFIG, xarchives) {
    XCONFIG += xarchives
    include($$PWD/../XArchive/xarchives.pri)
}

DISTFILES += \
    $$PWD/LICENSE \
    $$PWD/README.md \
    $$PWD/xdataconvertorwidget.cmake
