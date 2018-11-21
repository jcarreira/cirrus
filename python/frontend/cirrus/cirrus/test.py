from __future__ import print_function
import cirrus

from lda import LDA
from lr import LogisticRegression

import os
path_to_key = os.path.join(os.path.expanduser('~'),'jeff/joao_key.pem')

data_bucket = 'nytimes-lda'
model = 'model_v1'

k, s3_size, slice_size
n_workers,
lambda_size,
n_ps,
dataset,
key_name, key_path, # aws key
ps_ip_public, # public parameter server ip
ps_ip_private, # private parameter server ip
ps_ip_port,
ps_username, # parameter server VM username
minibatch_size,
progress_callback,

lda_task = LDA(
             k = 20,
             s3_size = 9000,
             slice_size = 500,
             n_workers = 1,
             n_ps = 1,
             worker_size = 128,
             dataset = nytimes-lda,
             progress_callback = print,
             key_name='joao_key.pem',
             key_path=path_to_key,
             ps_ip_public='ec2-34-212-6-172.us-west-2.compute.amazonaws.com',
             ps_ip_private='172.31.5.74',
             ps_username='ubuntu',
             minibatch_size=100,
             train_set=(1,2),
             )

try:
    lda_task.run()
except KeyboardInterrupt:
    lda_task.kill()
