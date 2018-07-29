#!/bin/bash

for cmd in "timeout 200 ./tests/test_travis/mnist_test/test_sm" "timeout 150 ./tests/test_travis/mnist_test/worker_sm"; do
  eval ${cmd} &
  sleep 10
done
