#!/bin/bash
g++ -c -std=c++11 -fPIC -I/usr/include/spinnaker main.cpp
g++ -std=c++11 -fPIC *.o -L/usr/lib -L/usr/local/lib `pkg-config --libs opencv` -lSpinnaker -o run
