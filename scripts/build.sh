#!/bin/sh

source /etc/lsb-release
echo "build on ${DISTRO_CODENAME}"

if [ ${DISTRO_CODENAME} == "focal" ]; then
    BUILD_DIR=blg
    # docker run -it -w /repo -v $(pwd):/repo ubuntu_focal bash
    #cd blg
    #g++ -std=c++2a -o demo_safe_get -I.. ../examples/demo_safe_get.cpp -Wall -Wextra && ./demo_safe_get
else
    BUILD_DIR=build
fi

cd $BUILD_DIR
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build . --config Debug -- -j $(nproc)