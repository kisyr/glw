#! /bin/sh
set -x
for file in *.cpp; do
    g++ -std=c++0x $file -lglfw -lGL -lGLEW -I.. && ./a.out || return 1
done 
return 0
