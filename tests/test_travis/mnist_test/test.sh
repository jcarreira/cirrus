#!/bin/bash

for cmd in "timeout 140 ./tests/test_travis/mnist_test/test_sm" "timeout 120 ./tests/test_travis/mnist_test/worker_sm"; do
  eval ${cmd} &
  sleep 10
done
