FROM ubuntu:xenial

ADD cirrus.tar /cirrus
WORKDIR /cirrus
RUN ls
RUN apt-get update
RUN apt-get install -y software-properties-common
RUN add-apt-repository -y ppa:ubuntu-toolchain-r/test
RUN apt-get update \
    && apt-get install -y \
        g++-7 clang-format-3.8 \
        libstdc++6 liblz4-dev \
        make wget \
        git cmake \
        byacc pkg-config \
        lbzip2 bzip2 \
        automake zlib1g-dev \
        libssl-dev libcurl4-nss-dev \
        bison libldap2-dev \
        libkrb5-dev autoconf-archive \
        uuid-dev libpulse-dev
RUN g++ --version; gcc --version
RUN ls; pwd
RUN ls third_party; pwd
RUN cd third_party; rm -rf aws-sdk-cpp gflags glog
RUN cd third_party; git clone https://github.com/aws/aws-sdk-cpp.git
RUN cd third_party; git clone https://github.com/gflags/gflags.git
RUN cd third_party; git clone https://github.com/google/glog.git
RUN ls third_party; pwd
RUN ls third_party/gflags; ls third_party/glog
RUN mv src/Makefile_ubuntu.am src/Makefile.am
RUN g++ --version; gcc --version
RUN ln -s -f /usr/bin/gcc-7 /usr/local/bin/gcc
RUN ln -s -f /usr/bin/g++-7 /usr/local/bin/g++
RUN g++ --version; gcc --version
RUN ./bootstrap.sh
RUN ls
RUN ls third_party/
RUN make -j 10
RUN .travis/check-git-clang-format-output.sh  
