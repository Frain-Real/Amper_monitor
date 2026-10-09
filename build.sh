#! /bin/bash

set -e
# Build app

if [ "x$1" = "x" ]; then
	make clean
    make 
else
	make clean
    make -f "Makefile.$1"
fi

# Files to upload
rm -rf .upload
mkdir -p .upload/

cp out/AM.bin .upload/"Amper_Monitor.bin"
