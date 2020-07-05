#!/bin/bash

SRC=/app/src
BUILD=/app/build

cd ${BUILD}
cmake ${SRC}
make