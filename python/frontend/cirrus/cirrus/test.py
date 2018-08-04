from __future__ import print_function
import cirrus

from lda import LDA
from lr import LogisticRegression

import os
path_to_key = os.path.join(os.path.expanduser('~'),'jeff/joao_key.pem')

data_bucket = 'nytimes-lda'
model = 'model_v1'

lda_task = LDA(
             # number of workers
             n_workers = 1,
             # number of parameter servers
             n_ps = 1,
             # worker size in MB
             worker_size = 128,
             # path to s3 bucket with input dataset
             dataset = data_bucket,
             # sgd update LR and epsilon
             learning_rate=0.01,
             epsilon=0.0001,
             progress_callback = print,
             # stop workload after these many seconds
             timeout = 0,
             # stop workload once we reach this loss
             threshold_loss=0,
             # resume execution from model stored in this s3 bucket
             resume_model = model,
             # aws key name
             key_name='joao_key.pem',
             # path to aws key
             key_path=path_to_key,
             # ip where ps lives
             ps_ip_public='ec2-34-212-6-172.us-west-2.compute.amazonaws.com',
             ps_ip_private='172.31.5.74',
             # username of VM
             ps_username='ubuntu',
             # choose between adagrad, sgd, nesterov, momentum
             opt_method = 'adagrad',
             # checkpoint model every x secs
             checkpoint_model = 60,
             #
             minibatch_size=1000,
             # model size
             model_bits=19,
             # whether to filter gradient weights
             use_grad_threshold=False,
             # threshold value
             grad_threshold=0.001,
             # range of training minibatches
             train_set=(1,9),
             # range of testing minibatches
             test_set=(835,840)
             )

try:
    lda_task.run()
except KeyboardInterrupt:
    lda_task.kill()
