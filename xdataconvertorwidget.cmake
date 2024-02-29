include_directories(${CMAKE_CURRENT_LIST_DIR})

include(${CMAKE_CURRENT_LIST_DIR}/../Formats/xdataconvertor.cmake)

set(XDATACONVERTORWIDGET_SOURCES
    ${XDATACONVERTOR_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/dialogxdataconvertorprocess.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dialogxdataconvertorprocess.h
    ${CMAKE_CURRENT_LIST_DIR}/xdataconvertorwidget.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xdataconvertorwidget.h
    ${CMAKE_CURRENT_LIST_DIR}/xdataconvertorwidget.ui
    ${CMAKE_CURRENT_LIST_DIR}/dialogxdataconvertor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dialogxdataconvertor.h
    ${CMAKE_CURRENT_LIST_DIR}/dialogxdataconvertor.ui
)
