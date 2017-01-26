set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/mqtt_live_objects)
	
file(GLOB_RECURSE ALL_SOURCE_FILES 
	${SOURCE_DIR}/iotsoftbox-mqtt-core/iotsoftbox-core/*.c
	${SOURCE_DIR}/iotsoftbox-mqtt-core/iotsoftbox-core/*.h
	${SOURCE_DIR}/iotsoftbox-mqtt-core/liveobjects-client/*.c
	${SOURCE_DIR}/iotsoftbox-mqtt-core/liveobjects-client/*.h
	${SOURCE_DIR}/platforms/*.c
	${SOURCE_DIR}/platforms/*.h
)

set(ALL_SOURCE_FILES  ${ALL_SOURCE_FILES})
add_custom_target(
  cpplint
  COMMAND cpplint
  --verbose=1
  --quiet ${ALL_SOURCE_FILES}
)
