cmake_minimum_required(VERSION 3.19)
project(microproject C)

set(CMAKE_C_STANDARD 11)

add_definitions(-DCURL_STATICLIB)

add_executable(microproject main.c transaction/transaction.c transaction/transaction.h cJSON/cJSON.h cJSON/cJSON.c transaction/transaction.c transaction/transaction.h stringList/stringList.c stringList/stringList.h features/features.c features/features.h)

find_package(CURL)
if (CURL_FOUND)
    include_directories(${CURL_INCLUDE_DIRS})
    target_link_libraries(microproject ${CURL_LIBRARIES})
else()
    MESSAGE(FATAL_ERROR "LibCurl not found")
endif(CURL_FOUND)

target_link_libraries(microproject ${LIBS})