#!/bin/bash
# script para compilar proyectos cmake ninja
rm -rf build/
mkdir build
cd build
cmake -G Ninja ..
ninja
