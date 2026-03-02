#!/bin/bash

cmake -S . -B build-linux
cmake -S . -B build-windows -DCMAKE_TOOLCHAIN_FILE=toolchain-windows.cmake