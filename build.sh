#!/bin/bash

set -e
set -x

# ./project_init.sh

CMAKE_OPTS=""
# CMAKE_OPTS="--trace-expand"

### For Release ###
# CMAKE_OPTS="${CMAKE_OPTS} -DCMAKE_BUILD_TYPE=Release"
# CXXFLAGS="-std=c++17 -march=native -mtune=native -Og -g3 -ggdb -Wextra -Wpedantic"

### For Debug ###
CMAKE_OPTS="${CMAKE_OPTS} -DCMAKE_BUILD_TYPE=Debug"
CXXFLAGS="-std=c++17 -march=native -mtune=native -Og -g3 -ggdb -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-unused-label -Wno-unused-value -Wno-unused-but-set-variable -Wno-unused-local-typedefs -Wno-unused-result -Wno-missing-field-initializers -Wno-missing-braces -Wno-missing-braces -Wno-missing-field-initializers"

# test -d "./opencv_build"

(
    mkdir -p ./build
    rm -rf ./build/*
    cd build
    CXXFLAGS=${CXXFLAGS} cmake ${CMAKE_OPTS} -B . -S ..
    make -j $(nproc)
)

(
    DISASM_BINARY_FILE=./build/bin/int/libtask_graph_cpp_LIB.a
    DISASM_OUTPUT_FILE=./build/disasm.s
    objdump -d -r -C --wide --insn-width=12 -M intel "${DISASM_BINARY_FILE}" > "${DISASM_OUTPUT_FILE}"
)
