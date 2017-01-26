set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/mqtt_live_objects)
	
file(GLOB_RECURSE ALL_SOURCE_FILES 
	${SOURCE_DIR}/iotsoftbox-mqtt-core/iotsoftbox-core/*.c
	${SOURCE_DIR}/iotsoftbox-mqtt-core/iotsoftbox-core/*.h
	${SOURCE_DIR}/iotsoftbox-mqtt-core/liveobjects-client/*.c
	${SOURCE_DIR}/iotsoftbox-mqtt-core/liveobjects-client/*.h
	${SOURCE_DIR}/platforms/*.c
	${SOURCE_DIR}/platforms/*.h
)

add_custom_target(
  cppcheck
  COMMAND /usr/bin/cppcheck
  --enable=warning,performance,portability,information,missingInclude
  --std=c++11
  --template="[{severity}][{id}] {message} {callstack} \(On {file}:{line}\)"
  --verbose
  --quiet ${ALL_SOURCE_FILES}
)
