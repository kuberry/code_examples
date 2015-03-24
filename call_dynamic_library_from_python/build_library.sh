#!/bin/bash
# from  http://www.yolinux.com/TUTORIALS/LibraryArchives-StaticAndDynamic.html
export FILENAME="hello"

mkdir -p lib; mkdir -p obj;

g++ -Wall -fPIC -c $FILENAME.cpp -o ./obj/$FILENAME.o
g++ -shared -Wl,-soname,lib$FILENAME.so.1 -o lib$FILENAME.so.1.0   ./obj/$FILENAME.o
mv lib$FILENAME.so.1.0 ./lib
ln -sf ./lib$FILENAME.so.1.0 ./lib/lib$FILENAME.so.1
ln -sf ./lib$FILENAME.so.1.0 ./lib/lib$FILENAME.so

