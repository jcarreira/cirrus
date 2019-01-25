#!/bin/bash

timeout 10 ./tests/test_travis_lda/load&
wait

timeout 60 ./tests/test_travis_lda/test_ps&
sleep 1

timeout 50 ./tests/test_travis_lda/w1&
sleep 1

timeout 50 ./tests/test_travis_lda/w2&
sleep 1
