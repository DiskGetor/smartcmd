#!/bin/bash



if [ $1 = 'arm' ]
then
	echo Build for arm target
	export CXX=arm-linux-gnueabihf-g++
	export CC=arm-linux-gnueabihf-gcc
else
	echo Build for desktop target
	export CXX=g++
	export CC=gcc
	if [ $1 = '32' ]
	then
		export ENV=32
	fi
fi


make()
{
  pathpat="(/[^/]*)+:[0-9]+"
  ccred=$(echo -e "\033[0;31m")
  ccyellow=$(echo -e "\033[0;33m")
  ccend=$(echo -e "\033[0m")
  /usr/bin/make "$@" 2>&1 | sed -E -e "/[Ee]rror[: ]/ s%$pathpat%$ccred&$ccend%g" -e "/[Ww]arning[: ]/ s%$pathpat%$ccyellow&$ccend%g"
  return ${PIPESTATUS[0]}
}

make clean
make