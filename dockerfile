FROM ubuntu:24.04

ENV APP_VERSION=1.0

RUN apt-get update && apt-get install -y --no-install-recommends build-essential git cmake autoconf libtool pkg-config && rm -rf /var/lib/apt/lists/*
# libboost-all-dev

WORKDIR /app 
#WORKDIR ~/developer/overbond/overbond

# limit app permissions
#RUN groupadd -r appuser && useradd --no-log-init -r -g appuser appuser
#USER appuser

COPY CMakeLists.txt main.cpp inputbonds.csv bond.* .

RUN cmake . && make

CMD ["./overbondtest", "inputbonds.csv"]
