project("iotsoftbox-mqtt-linux" C)
cmake_minimum_required(VERSION 3.0)

# Create various directory in the build folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Lib inclusion

# Thread
find_package(Threads)

# MbedTLS
add_subdirectory(lib/mbedtls)
link_directories(lib/mbedtls/library)

# JSMN
add_library(jsmn lib/jsmn/jsmn.c)

# MQTTPacket
set(MQTTPACKET_PATH lib/paho.mqtt.embedded-c/MQTTPacket/src)
file(GLOB MQTTPACKET_SOURCE ${MQTTPACKET_PATH}/*.c)
add_library(MQTTPacket ${MQTTPACKET_SOURCE})

set(COMMON_LIB_LIST ${CMAKE_THREAD_LIBS_INIT} MQTTPacket jsmn mbedtls mbedcrypto mbedx509 m)

# The examples
# Comment an example will disable the build.
add_subdirectory(examples/liveobjects_sample_basic)
#add_subdirectory(examples/liveobjects_sample_update)
#add_subdirectory(examples/liveobjects_sample_minimal)

# Cmake-utilities
#include(cmake-utilities/cppcheck.cmake)
#include(cmake-utilities/cpplint.cmake)
