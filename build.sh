#!/usr/bin/env bash

cd build
cmake ..
cmake --build . -j 10
cd ..