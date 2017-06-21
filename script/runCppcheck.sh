#!/bin/sh

if [ -d mqtt_live_objects ]
then
sourceCode=`find mqtt_live_objects -name '*.[ch]'`
example=`find examples -name '*.[ch]'`
else 
sourceCode=`find ../mqtt_live_objects -name '*.[ch]'`
example=`find ../examples -name '*.[ch]'`
fi

cppcheck --enable=warning,performance,portability,information,missingInclude --std=c++11 --template="[{severity}][{id}] {message} {callstack}" --verbose --quiet $sourceCode $example
