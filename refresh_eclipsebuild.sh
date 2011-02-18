#!/bin/bash

git clean -fdX
cmake -DCMAKE_CXX_COMPILER=$(which clang++) -G "Eclipse CDT4 - Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug .
#cmake -G "Eclipse CDT4 - Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug .
