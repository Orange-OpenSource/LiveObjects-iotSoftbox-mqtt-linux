# Define various var name
set(EXECUTABLE_NAME basic)
set(CORE_LIB loc_core_for_${EXECUTABLE_NAME})
set(SOURCE_PATH ${CMAKE_SOURCE_DIR}/mqtt_live_objects)
set(PLATFORM_PATH ${CMAKE_SOURCE_DIR}/mqtt_live_objects/platforms/linux)
set(PLATFORM_IOTSOFTBOX_PATH ${PLATFORM_PATH}/iotsoftbox-linux)
set(LIB_PATH ${CMAKE_SOURCE_DIR}/lib)

#Bring the headers
include_directories(${SOURCE_PATH})
include_directories(${SOURCE_PATH}/LiveObjects-iotSoftbox-mqtt-core)
include_directories(${LIB_PATH})
include_directories(${LIB_PATH}/paho.mqtt.embedded-c/MQTTPacket/src)
include_directories(${LIB_PATH}/mbedtls/include)
include_directories(${PLATFORM_PATH})
include_directories(${CMAKE_SOURCE_DIR}/mbedtls_configs)

# Bring the Config
include_directories(.)

# Make a List of all source files
file(GLOB_RECURSE ALL_SOURCE ${SOURCE_PATH}/*.c)

# Generate the library from the sources
add_library(${CORE_LIB} ${ALL_SOURCE})

# You can change the name of the c file but don't forget to report the modification here
# Set the executable name and create it
add_executable(${EXECUTABLE_NAME} ${EXECUTABLE_NAME}.c)

# Link edition configuration
target_link_libraries(${EXECUTABLE_NAME}
 ${CORE_LIB}
 ${COMMON_LIB_LIST}
)
