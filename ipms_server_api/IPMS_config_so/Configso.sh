#/bin/bash

g++  -fPIC -shared -o config.so Config.cpp

 g++ -o sotest sotest.cpp ./config.so 
