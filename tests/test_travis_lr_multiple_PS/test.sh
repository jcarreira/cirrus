#!/bin/bash

timeout 60 ./tests/test_travis_lr_multiple_PS/test_ps & 
sleep 1
timeout 60 ./tests/test_travis_lr_multiple_PS/test_ps2 & 
sleep 1
timeout 50 ./tests/test_travis_lr_multiple_PS/worker & 
sleep 1
timeout 50 ./tests/test_travis_lr_multiple_PS/worker &
sleep 1
