cmake_minimum_required(VERSION 3.14)



project(ZmqServer LANGUAGES CXX)



set(CMAKE_AUTOUIC ON)

set(CMAKE_AUTOMOC ON)

set(CMAKE_AUTORCC ON)



set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(Qt6 COMPONENTS Sql REQUIRED)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libzmq/include ${CMAKE_CURRENT_SOURCE_DIR}/OpenSSL/include)


add_executable(ZmqServer

  main.cpp
  dao.h dao.cpp
  zmqserver.h zmqserver.cpp
)

target_link_libraries(ZmqServer Qt${QT_VERSION_MAJOR}::Core
    ${CMAKE_CURRENT_SOURCE_DIR}/libzmq/lib/libzmq-v142-mt-4_3_5.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/OpenSSL/lib/libcrypto.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/OpenSSL/lib/libssl.lib
    crypt32.lib advapi32.lib ws2_32.lib user32.lib
    Qt6::Sql
)




install(TARGETS ZmqServer

    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

