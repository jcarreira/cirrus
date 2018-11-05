#!/bin/bash

for cmd in "timeout 60 ./tests/test_travis_lr/test_ps" "timeout 50 ./tests/test_travis_lr/worker" "timeout 50 ./tests/test_travis_lr/worker"; do
  eval ${cmd} &
  sleep 1
done
