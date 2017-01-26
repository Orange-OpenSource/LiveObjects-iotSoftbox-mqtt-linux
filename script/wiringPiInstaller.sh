#!/bin/sh

cross1="arm-linux-gnueabi"
crosscompiler1="arm-linux-gnueabi-gcc"

cross2="arm-linux-gnueabihf"
crosscompiler2="arm-linux-gnueabihf-gcc"

git clone https://github.com/WiringPi/WiringPi.git
cd WiringPi
./build

MAKEFILES=`find . -name "Makefile"`

for i in $MAKEFILES; do
	sed -i s/gcc/$crosscompiler1/ $i
	sed -i s:\=/usr:\=/usr/$cross1: $i
	sed -i s:PREFIX\?\=/local:"": $i
done

./build clean
./build

for j in $MAKEFILES; do
	sed -i s/gcc/$crosscompiler2/ $i
	sed -i s:\=/usr:\=/usr/$cross2: $i
	sed -i s:PREFIX\?\=/local:"": $i
done

./build clean
./build
cd ../
rm -rf WiringPi
