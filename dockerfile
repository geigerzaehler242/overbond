FROM ubuntu:24.04

ENV APP_VERSION=1.0

RUN apt-get update && apt-get install -y build-essential git cmake autoconf libtool pkg-config && rm -rf /var/lib/apt/lists/* 
# libboost-all-dev

WORKDIR /app 
#WORKDIR ~/developer/overbond/overbond

COPY CMakeLists.txt main.cpp inputbonds.csv bond.* .

RUN cmake . && make

CMD ["./overbondtest", "inputbonds.csv"]
