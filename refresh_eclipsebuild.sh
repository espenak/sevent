#!/bin/bash

cmake -DCMAKE_CXX_COMPILER=$(which clang++) -G "Eclipse CDT4 - Unix Makefiles" .
