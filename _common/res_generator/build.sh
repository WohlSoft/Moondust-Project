#!/bin/bash

mkdir -p ./bin
g++ -std=c++11 -c -I../IniProcessor/ res_generator.cpp -o bin/res_generator.o -Wall
g++ -std=c++11 -c -I../IniProcessor/ ../IniProcessor/ini_processing.cpp -o bin/ini_processing.o -Wall

g++ bin/ini_processing.o bin/res_generator.o -o bin/res_generator

# Build resources!
bin/res_generator

rm -Rf ./bin

