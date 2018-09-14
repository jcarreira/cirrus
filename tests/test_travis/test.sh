#!/bin/bash

for cmd in "(timeout 100 ./src/parameter_server --nworkers=10 --rank=1 --config=\"configs/criteo_kaggle.cfg\" --ps_port=1341) || true" "(timeout 75 ./src/parameter_server --nworkers=10 --rank=3 --config=\"configs/criteo_kaggle.cfg\" --ps_ip=\"127.0.0.1\" --ps_port=1341) || true" "(timeout 75 ./src/parameter_server --nworkers=10 --rank=3 --config=\"configs/criteo_kaggle.cfg\" --ps_ip=\"127.0.0.1\" --ps_port=1341) || true"; do
  eval ${cmd} &
  sleep 10
done
