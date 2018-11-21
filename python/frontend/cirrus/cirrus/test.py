from __future__ import print_function
import cirrus

from lda import LDA
from lr import LogisticRegression

import os
path_to_key = os.path.join(os.path.expanduser('~'),'jeff/joao_key.pem')

data_bucket = 'nytimes-lda'
model = 'model_v1'

lda_task = LDA(
             k = 20,
             s3_size = 9000,
             slice_size = 500,
             n_workers = 1,
             lambda_size = 128,
             n_ps = 1,
             dataset = "nytimes-lda",
             progress_callback = print,
             key_name='joao_key.pem',
             key_path=path_to_key,
             ps_ip_public='18.237.161.88',
             ps_ip_private='172.31.11.53',
             ps_ip_port = 1337,
             ps_username='ubuntu',
             minibatch_size=100,
             train_set=(1,2)
             )

try:
    lda_task.run()
except KeyboardInterrupt:
    lda_task.kill()
