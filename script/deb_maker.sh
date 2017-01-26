#!/bin/sh

echo "-- Minimal deb maker assistant --"

exe_name=$1

path_to_bin="../build/bin/"

if [ ! -f "${path_to_bin}${exe_name}" ]; then
	echo "ERROR: The executable doesn't exist"
	echo "Did you run make or specify the right name?"
	exit -1
fi

#Default value
package=$exe_name
version="1.0-0"
section="base"
priority="optional"
architecture="amd64"
depends=""
maintainer="Your Name <you@email.com>"
description="Hello World
 When you need some sunshine, just run this
 small program!"

#TODO Check if the control file exist
#if yes read it and parse it

echo -n "Package [$package]: "; read package_tmp
if [ ! -z $package_tmp ]; then
	package=$package_tmp
fi

echo -n "Version [$version]: "; read version_tmp
if [ ! -z $version_tmp ]; then
	version=$version_tmp
fi
echo -n "Section [$section]: "; read section_tmp
if [ ! -z $section_tmp ]; then
	section=$section_tmp
fi
echo -n "Priority [$priority]: "; read priority_tmp
if [ ! -z $priority_tmp ]; then
	priority=$priority_tmp
fi
echo -n "Architecture [$architecture]: "; read architecture_tmp
if [ ! -z $architecture_tmp ]; then
	architecture=$architecture_tmp
fi
echo -n "Depends [$depends]: "; read depends_tmp
if [ ! -z $depends ]; then
	depends=$depends
fi
echo -n "Maintainer [$maintainer]: "; read maintainer_tmp
if [ ! -z $maintainer_tmp ]; then
	maintainer=$maintainer_tmp
fi
echo -n "Description [$description]: "; read description_tmp
if [ ! -z $description_tmp ]; then
	description=$description_tmp
fi

#Creation of the tree
deb_name=$package"_"$version
mkdir -p $deb_name/DEBIAN
mkdir -p $deb_name/usr/local/bin
cp ${path_to_bin}${exe_name} $deb_name/usr/local/bin

echo "Package: $package" > $deb_name/DEBIAN/control
echo "Version: $version" >> $deb_name/DEBIAN/control
echo "Section: $section" >> $deb_name/DEBIAN/control
echo "Priority: $priority" >> $deb_name/DEBIAN/control
echo "Architecture: $architecture" >> $deb_name/DEBIAN/control
echo "Depends: $depends" >> $deb_name/DEBIAN/control
echo "Maintainer: $maintainer" >> $deb_name/DEBIAN/control
echo "Description: $description" >> $deb_name/DEBIAN/control

#Building the deb
dpkg --build $deb_name

echo -n "Do you want to remove Deb building folder ? [y/n]: "; read remove_tmp
if [ "y"="$remove_tmp" ] || [ "Y"="$remove_tmp" ]; then
	rm -rf $deb_name;
fi
