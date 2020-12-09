FROM ubuntu:20.04

RUN apt update
RUN apt install -y valgrind libboost-all-dev cmake libjsoncpp-dev git