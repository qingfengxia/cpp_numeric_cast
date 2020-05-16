#!/bin/sh

if [ -f /etc/lsb-release ]; then
    source /etc/lsb-release
else
    export DISTRIB_CODENAME="unknown"
fi
echo "build on linux distro: ${DISTRIB_CODENAME}"

if [[ ${DISTRIB_CODENAME} == "focal" ]]; then
    BUILD_DIR=blg
    # docker run  -ti  -u root -w /work -v $(pwd):/work ubuntu_focal:latest  /bin/bash
    # CMake on ubuntu focal should have support CMAKE_CXX_STANDARD=20
    cd $BUILD_DIR
    cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_STANDARD=20
    cmake --build . --config Debug -- -j $(nproc)
else
    BUILD_DIR=build
    cd $BUILD_DIR
    # for local dev, debug and coverage on
    cmake .. -DCMAKE_BUILD_TYPE=Debug
    #make  -j $(nproc)  # this command does not work on Windows
    cmake --build . -- -j $(nproc)
fi

