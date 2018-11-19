#!/bin/bash

eval "timeout 60 ./tests/test_travis_mf_multiple_PS/test_ps" &
sleep 1
eval "timeout 50 ./tests/test_travis_mf_multiple_PS/worker" & 
sleep 1
eval "timeout 50 ./tests/test_travis_mf_multiple_PS/worker" &
sleep 1
