include_directories(${CMAKE_CURRENT_LIST_DIR})

if (NOT DEFINED XDATACONVERTOR_SOURCES)
    include(${CMAKE_CURRENT_LIST_DIR}/../Formats/xdataconvertor.cmake)
    set(XDATACONVERTORWIDGET_SOURCES ${XDATACONVERTORWIDGET_SOURCES} ${XDATACONVERTOR_SOURCES})
endif()

# XDecompress (XArchive) powers the Decompress family in the widget layer.
if (NOT DEFINED XARCHIVES_SOURCES)
    include(${CMAKE_CURRENT_LIST_DIR}/../XArchive/xarchives.cmake)
    set(XDATACONVERTORWIDGET_SOURCES ${XDATACONVERTORWIDGET_SOURCES} ${XARCHIVES_SOURCES})
endif()

set(XDATACONVERTORWIDGET_SOURCES
    ${XDATACONVERTORWIDGET_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/xdataconvertorwidget.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xdataconvertorwidget.h
    ${CMAKE_CURRENT_LIST_DIR}/xdataconvertorwidget.ui
    ${CMAKE_CURRENT_LIST_DIR}/xdataconvertordecompress.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xdataconvertordecompress.h
    ${CMAKE_CURRENT_LIST_DIR}/dialogxdataconvertor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dialogxdataconvertor.h
    ${CMAKE_CURRENT_LIST_DIR}/dialogxdataconvertor.ui
)
