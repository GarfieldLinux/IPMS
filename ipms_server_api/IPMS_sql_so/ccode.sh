#/bin/bash

g++ -fPIC -shared -o cppTestLib.so CppCode.cpp  
gcc  -o ccode CCode.c ./cppTestLib.so
 ./ccode
