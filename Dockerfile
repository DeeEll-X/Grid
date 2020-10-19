FROM ubuntu:18.04

RUN apt update
RUN apt install -y valgrind libboost-all-dev cmake
RUN apt install -y git