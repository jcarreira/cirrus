#!/bin/bash

for cmd in "timeout 100 ./src/parameter_server --nworkers=10 --rank=1 --config=\"configs/softmax_config.cfg\"" "timeout 75 ./src/parameter_server --nworkers=10 --rank=3 --config=\"configs/softmax_config.cfg\" --ps_ip=\"127.0.0.1\""; do
  eval ${cmd} &
  sleep 10
done
