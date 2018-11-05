#!/bin/bash

for cmd in "timeout 60 ./tests/test_travis_mf_multiplePS/test_ps" "timeout 50 ./tests/test_travis_mf_multiplePS/worker" "timeout 50 ./tests/test_travis_mf_multiplePS/worker"; do
  eval ${cmd} &
  sleep 1
done
