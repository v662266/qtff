FROM ubuntu:latest

ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update && \
    apt-get install -y \
    cmake g++ make \
    qt5-default qtbase5-dev \
    libavformat-dev libavfilter-dev libavcodec-dev 

RUN mkdir -p /app/src
WORKDIR /app/build
COPY ./build_inside_docker.sh /root/bin/

#ENTRYPOINT sh /root/bin/build_inside_docker.sh
