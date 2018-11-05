#!/bin/bash

for cmd in "timeout 60 ./tests/test_travis_lr_multiplePS/test_ps" "timeout 60 ./tests/test_travis_lr_multiplePS/test_ps2" "timeout 50 ./tests/test_travis_lr_multiplePS/worker" "timeout 50 ./tests/test_travis_lr_multiplePS/worker"; do
  eval ${cmd} &
  sleep 1
done
