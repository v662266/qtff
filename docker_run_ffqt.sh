#!/bin/bash

xhost +local:docker

QT_GRAPHICSSYSTEM="native" \
docker run -it \
        -e DISPLAY=$DISPLAY -v /tmp/.X11-unix:/tmp/.X11-unix \
        -v $(pwd):/app/src \
        -v $(pwd)/output:/app/build \
        qtbuild /app/build/ffqt http://i.imgur.com/gQghRNd.mp4
