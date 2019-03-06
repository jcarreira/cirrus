#!/bin/bash

cd ./tests/test_data
./nytimes.sh

mv nyt_data.txt nytimes.txt
mv nyt_vocab.dat nytimes_vocab.txt

cd ../../
#
timeout 10 ./tests/test_travis_lda/load&
wait

timeout 60 ./tests/test_travis_lda/test_ps&
sleep 1

timeout 50 ./tests/test_travis_lda/w1&
sleep 1

timeout 50 ./tests/test_travis_lda/w2&
sleep 1

rm ./tests/test_data/nytimes.txt
rm ./tests/test_data/nytimes_vocab.txt
