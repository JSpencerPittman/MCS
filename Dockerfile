FROM ubuntu:jammy

RUN apt-get update && apt-get install -y \
    g++ \
    git \
    cmake \
    build-essential \
    cmake \
    gdb \
    wget \
    && rm -rf /var/lib/apt/lists/*