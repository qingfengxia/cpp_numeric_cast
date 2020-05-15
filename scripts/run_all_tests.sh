#!/bin/sh

# must run the app in the folder where *.gcno file are located
for demo in bin/demo*; do
    echo "run the test program: ${demo}"
    sh -c "./${demo}"
    if [[ $? -ne 0 ]]; then
        echo "demo exit with error, ignore for the moment"
    fi
done

# run Test and generate coverage report
ctest -j $(nproc) --output-on-failure