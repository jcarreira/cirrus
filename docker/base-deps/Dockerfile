FROM ubuntu:xenial

ADD cirrus.tar /cirrus
WORKDIR /cirrus
RUN ls
RUN apt-get update
RUN apt-get install -y software-properties-common
RUN add-apt-repository -y ppa:ubuntu-toolchain-r/test
RUN apt-get update
RUN apt-get install -y g++-7 clang-format-3.8 libstdc++6 liblz4-dev make wget git cmake
RUN ls; pwd
RUN ./bootstrap.sh
RUN make -j 10
RUN mv src/Makefile_ubuntu.am src/Makefile.am
RUN .travis/check-git-clang-format-output.sh  

