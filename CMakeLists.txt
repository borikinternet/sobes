cmake_minimum_required(VERSION 3.0)
project(iteko_sobes)

set(CMAKE_CXX_STANDARD 11)

find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Boost REQUIRED
        COMPONENTS thread chrono)

add_executable(client src/client.c)
target_include_directories(client
        PRIVATE include
        PRIVATE ${OPENSSL_INCLUDE_DIR}
        )
target_link_libraries(client
        ${OPENSSL_CRYPTO_LIBRARIES})

add_executable(server src/server.cpp src/Getter.cpp src/Parser.cpp src/Buffer.cpp)
target_include_directories(server
        PRIVATE include
        PRIVATE ${BOOST_INCLUDEDIR}
        )
target_link_libraries(server
        ${OPENSSL_CRYPTO_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
        ${Boost_LIBRARIES})

