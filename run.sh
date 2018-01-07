#!/usr/bin/env bash
cd build && cmake .. -DCMAKE_BUILD_TYPE=Debug
make UnitTests
./UnitTests

