#!/bin/bash
#git archive -o ./docker/base-deps/cirrus.tar $(git rev-parse HEAD)

#ROOT_DIR=$(cd "$(dirname "${BASH_SOURCE:-$0}")"; pwd)
#echo $ROOT_DIR

#cd ..
./bootstrap.sh
tar -czf cirrus.tar . --exclude=./*.gz
mv cirrus.tar docker/base-deps
##docker build docker/base-deps
