#!/bin/bash
git archive -o ./docker/base-deps/cirrus.tar $(git rev-parse HEAD)
docker build docker/base-deps/ -t docker/base

