echo "input_path: /mnt/efs/criteo_kaggle/train.csv 
input_type: csv
num_classes: 2 
num_features: 13 
limit_cols: 14 
normalize: 1 
limit_samples: 50000000 
s3_size: 50000 
use_bias: 1 
model_type: LogisticRegression 
minibatch_size: 20 
learning_rate: 0.010000 
epsilon: 0.000100 
model_bits: 19 
s3_bucket: cirrus-criteo-kaggle-19b-random 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 84-167,168-251,252-335,336-419,420-503,504-587,588-671,672-755,756-840 
test_set: 0-83" > config_1337.txt

nohup ./parameter_server --config config_1337.txt --nworkers 40 --rank 1 --ps_port 1337 &> ps_out_1337 & 

nohup ./parameter_server --config config_1337.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 1337 &> error_out_1337 &

echo "input_path: /mnt/efs/criteo_kaggle/train.csv 
input_type: csv
num_classes: 2 
num_features: 13 
limit_cols: 14 
normalize: 1 
limit_samples: 50000000 
s3_size: 50000 
use_bias: 1 
model_type: LogisticRegression 
minibatch_size: 20 
learning_rate: 0.010000 
epsilon: 0.000100 
model_bits: 19 
s3_bucket: cirrus-criteo-kaggle-19b-random 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-83,168-251,252-335,336-419,420-503,504-587,588-671,672-755,756-840 
test_set: 84-167" > config_1339.txt

nohup ./parameter_server --config config_1339.txt --nworkers 40 --rank 1 --ps_port 1339 &> ps_out_1339 & 

nohup ./parameter_server --config config_1339.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 1339 &> error_out_1339 &

echo "input_path: /mnt/efs/criteo_kaggle/train.csv 
input_type: csv
num_classes: 2 
num_features: 13 
limit_cols: 14 
normalize: 1 
limit_samples: 50000000 
s3_size: 50000 
use_bias: 1 
model_type: LogisticRegression 
minibatch_size: 20 
learning_rate: 0.010000 
epsilon: 0.000100 
model_bits: 19 
s3_bucket: cirrus-criteo-kaggle-19b-random 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-83,84-167,252-335,336-419,420-503,504-587,588-671,672-755,756-840 
test_set: 168-251" > config_1341.txt

nohup ./parameter_server --config config_1341.txt --nworkers 40 --rank 1 --ps_port 1341 &> ps_out_1341 & 

nohup ./parameter_server --config config_1341.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 1341 &> error_out_1341 &

echo "input_path: /mnt/efs/criteo_kaggle/train.csv 
input_type: csv
num_classes: 2 
num_features: 13 
limit_cols: 14 
normalize: 1 
limit_samples: 50000000 
s3_size: 50000 
use_bias: 1 
model_type: LogisticRegression 
minibatch_size: 20 
learning_rate: 0.010000 
epsilon: 0.000100 
model_bits: 19 
s3_bucket: cirrus-criteo-kaggle-19b-random 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-83,84-167,168-251,336-419,420-503,504-587,588-671,672-755,756-840 
test_set: 252-335" > config_1343.txt

nohup ./parameter_server --config config_1343.txt --nworkers 40 --rank 1 --ps_port 1343 &> ps_out_1343 & 

nohup ./parameter_server --config config_1343.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 1343 &> error_out_1343 &

echo "input_path: /mnt/efs/criteo_kaggle/train.csv 
input_type: csv
num_classes: 2 
num_features: 13 
limit_cols: 14 
normalize: 1 
limit_samples: 50000000 
s3_size: 50000 
use_bias: 1 
model_type: LogisticRegression 
minibatch_size: 20 
learning_rate: 0.010000 
epsilon: 0.000100 
model_bits: 19 
s3_bucket: cirrus-criteo-kaggle-19b-random 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-83,84-167,168-251,252-335,420-503,504-587,588-671,672-755,756-840 
test_set: 336-419" > config_1345.txt

nohup ./parameter_server --config config_1345.txt --nworkers 40 --rank 1 --ps_port 1345 &> ps_out_1345 & 

nohup ./parameter_server --config config_1345.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 1345 &> error_out_1345 &

echo "input_path: /mnt/efs/criteo_kaggle/train.csv 
input_type: csv
num_classes: 2 
num_features: 13 
limit_cols: 14 
normalize: 1 
limit_samples: 50000000 
s3_size: 50000 
use_bias: 1 
model_type: LogisticRegression 
minibatch_size: 20 
learning_rate: 0.010000 
epsilon: 0.000100 
model_bits: 19 
s3_bucket: cirrus-criteo-kaggle-19b-random 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-83,84-167,168-251,252-335,336-419,504-587,588-671,672-755,756-840 
test_set: 420-503" > config_1347.txt

nohup ./parameter_server --config config_1347.txt --nworkers 40 --rank 1 --ps_port 1347 &> ps_out_1347 & 

nohup ./parameter_server --config config_1347.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 1347 &> error_out_1347 &

echo "input_path: /mnt/efs/criteo_kaggle/train.csv 
input_type: csv
num_classes: 2 
num_features: 13 
limit_cols: 14 
normalize: 1 
limit_samples: 50000000 
s3_size: 50000 
use_bias: 1 
model_type: LogisticRegression 
minibatch_size: 20 
learning_rate: 0.010000 
epsilon: 0.000100 
model_bits: 19 
s3_bucket: cirrus-criteo-kaggle-19b-random 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-83,84-167,168-251,252-335,336-419,420-503,588-671,672-755,756-840 
test_set: 504-587" > config_1349.txt

nohup ./parameter_server --config config_1349.txt --nworkers 40 --rank 1 --ps_port 1349 &> ps_out_1349 & 

nohup ./parameter_server --config config_1349.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 1349 &> error_out_1349 &

echo "input_path: /mnt/efs/criteo_kaggle/train.csv 
input_type: csv
num_classes: 2 
num_features: 13 
limit_cols: 14 
normalize: 1 
limit_samples: 50000000 
s3_size: 50000 
use_bias: 1 
model_type: LogisticRegression 
minibatch_size: 20 
learning_rate: 0.010000 
epsilon: 0.000100 
model_bits: 19 
s3_bucket: cirrus-criteo-kaggle-19b-random 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-83,84-167,168-251,252-335,336-419,420-503,504-587,672-755,756-840 
test_set: 588-671" > config_1351.txt

nohup ./parameter_server --config config_1351.txt --nworkers 40 --rank 1 --ps_port 1351 &> ps_out_1351 & 

nohup ./parameter_server --config config_1351.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 1351 &> error_out_1351 &

echo "input_path: /mnt/efs/criteo_kaggle/train.csv 
input_type: csv
num_classes: 2 
num_features: 13 
limit_cols: 14 
normalize: 1 
limit_samples: 50000000 
s3_size: 50000 
use_bias: 1 
model_type: LogisticRegression 
minibatch_size: 20 
learning_rate: 0.010000 
epsilon: 0.000100 
model_bits: 19 
s3_bucket: cirrus-criteo-kaggle-19b-random 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-83,84-167,168-251,252-335,336-419,420-503,504-587,588-671,756-840 
test_set: 672-755" > config_1353.txt

nohup ./parameter_server --config config_1353.txt --nworkers 40 --rank 1 --ps_port 1353 &> ps_out_1353 & 

nohup ./parameter_server --config config_1353.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 1353 &> error_out_1353 &

echo "input_path: /mnt/efs/criteo_kaggle/train.csv 
input_type: csv
num_classes: 2 
num_features: 13 
limit_cols: 14 
normalize: 1 
limit_samples: 50000000 
s3_size: 50000 
use_bias: 1 
model_type: LogisticRegression 
minibatch_size: 20 
learning_rate: 0.010000 
epsilon: 0.000100 
model_bits: 19 
s3_bucket: cirrus-criteo-kaggle-19b-random 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-83,84-167,168-251,252-335,336-419,420-503,504-587,588-671,672-755 
test_set: 756-840" > config_1355.txt

nohup ./parameter_server --config config_1355.txt --nworkers 40 --rank 1 --ps_port 1355 &> ps_out_1355 & 

nohup ./parameter_server --config config_1355.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 1355 &> error_out_1355 &

