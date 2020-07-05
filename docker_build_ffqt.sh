#!/bin/bash

docker run -it \
        -v $(pwd):/app/src \
        -v $(pwd)/output:/app/build \
        qtbuild \
        sh /root/bin/build_inside_docker.sh
