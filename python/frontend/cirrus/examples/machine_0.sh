echo "load_input_path: /mnt/efs/mnist/train_mnist1.csv 
load_input_type: csv
num_classes: 10 
num_features: 784 
limit_cols: 1000 
normalize: 1 
limit_samples: 50000000 
s3_size: 1000 
use_bias: 1 
model_type: Softmax 
minibatch_size: 20 
learning_rate: 0.000050 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1337.txt

nohup ./parameter_server --config config_1337.txt --nworkers 20 --rank 1 --ps_port 1337 &> ps_out_1337 & 

nohup ./parameter_server --config config_1337.txt --nworkers 10 --rank 2 --ps_ip 172.31.14.190 --ps_port 1337 &> error_out_1337 &

echo "load_input_path: /mnt/efs/mnist/train_mnist1.csv 
load_input_type: csv
num_classes: 10 
num_features: 784 
limit_cols: 1000 
normalize: 1 
limit_samples: 50000000 
s3_size: 1000 
use_bias: 1 
model_type: Softmax 
minibatch_size: 20 
learning_rate: 0.000050 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1341.txt

nohup ./parameter_server --config config_1341.txt --nworkers 20 --rank 1 --ps_port 1341 &> ps_out_1341 & 

nohup ./parameter_server --config config_1341.txt --nworkers 10 --rank 2 --ps_ip 172.31.14.190 --ps_port 1341 &> error_out_1341 &

echo "load_input_path: /mnt/efs/mnist/train_mnist1.csv 
load_input_type: csv
num_classes: 10 
num_features: 784 
limit_cols: 1000 
normalize: 1 
limit_samples: 50000000 
s3_size: 1000 
use_bias: 1 
model_type: Softmax 
minibatch_size: 20 
learning_rate: 0.000045 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1345.txt

nohup ./parameter_server --config config_1345.txt --nworkers 20 --rank 1 --ps_port 1345 &> ps_out_1345 & 

nohup ./parameter_server --config config_1345.txt --nworkers 10 --rank 2 --ps_ip 172.31.14.190 --ps_port 1345 &> error_out_1345 &

echo "load_input_path: /mnt/efs/mnist/train_mnist1.csv 
load_input_type: csv
num_classes: 10 
num_features: 784 
limit_cols: 1000 
normalize: 1 
limit_samples: 50000000 
s3_size: 1000 
use_bias: 1 
model_type: Softmax 
minibatch_size: 20 
learning_rate: 0.000042 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1349.txt

nohup ./parameter_server --config config_1349.txt --nworkers 20 --rank 1 --ps_port 1349 &> ps_out_1349 & 

nohup ./parameter_server --config config_1349.txt --nworkers 10 --rank 2 --ps_ip 172.31.14.190 --ps_port 1349 &> error_out_1349 &

echo "load_input_path: /mnt/efs/mnist/train_mnist1.csv 
load_input_type: csv
num_classes: 10 
num_features: 784 
limit_cols: 1000 
normalize: 1 
limit_samples: 50000000 
s3_size: 1000 
use_bias: 1 
model_type: Softmax 
minibatch_size: 20 
learning_rate: 0.000042 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1353.txt

nohup ./parameter_server --config config_1353.txt --nworkers 20 --rank 1 --ps_port 1353 &> ps_out_1353 & 

nohup ./parameter_server --config config_1353.txt --nworkers 10 --rank 2 --ps_ip 172.31.14.190 --ps_port 1353 &> error_out_1353 &

echo "load_input_path: /mnt/efs/mnist/train_mnist1.csv 
load_input_type: csv
num_classes: 10 
num_features: 784 
limit_cols: 1000 
normalize: 1 
limit_samples: 50000000 
s3_size: 1000 
use_bias: 1 
model_type: Softmax 
minibatch_size: 20 
learning_rate: 0.000038 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1357.txt

nohup ./parameter_server --config config_1357.txt --nworkers 20 --rank 1 --ps_port 1357 &> ps_out_1357 & 

nohup ./parameter_server --config config_1357.txt --nworkers 10 --rank 2 --ps_ip 172.31.14.190 --ps_port 1357 &> error_out_1357 &

echo "load_input_path: /mnt/efs/mnist/train_mnist1.csv 
load_input_type: csv
num_classes: 10 
num_features: 784 
limit_cols: 1000 
normalize: 1 
limit_samples: 50000000 
s3_size: 1000 
use_bias: 1 
model_type: Softmax 
minibatch_size: 20 
learning_rate: 0.000036 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1361.txt

nohup ./parameter_server --config config_1361.txt --nworkers 20 --rank 1 --ps_port 1361 &> ps_out_1361 & 

nohup ./parameter_server --config config_1361.txt --nworkers 10 --rank 2 --ps_ip 172.31.14.190 --ps_port 1361 &> error_out_1361 &

echo "load_input_path: /mnt/efs/mnist/train_mnist1.csv 
load_input_type: csv
num_classes: 10 
num_features: 784 
limit_cols: 1000 
normalize: 1 
limit_samples: 50000000 
s3_size: 1000 
use_bias: 1 
model_type: Softmax 
minibatch_size: 20 
learning_rate: 0.000036 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1365.txt

nohup ./parameter_server --config config_1365.txt --nworkers 20 --rank 1 --ps_port 1365 &> ps_out_1365 & 

nohup ./parameter_server --config config_1365.txt --nworkers 10 --rank 2 --ps_ip 172.31.14.190 --ps_port 1365 &> error_out_1365 &

echo "load_input_path: /mnt/efs/mnist/train_mnist1.csv 
load_input_type: csv
num_classes: 10 
num_features: 784 
limit_cols: 1000 
normalize: 1 
limit_samples: 50000000 
s3_size: 1000 
use_bias: 1 
model_type: Softmax 
minibatch_size: 20 
learning_rate: 0.000033 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1369.txt

nohup ./parameter_server --config config_1369.txt --nworkers 20 --rank 1 --ps_port 1369 &> ps_out_1369 & 

nohup ./parameter_server --config config_1369.txt --nworkers 10 --rank 2 --ps_ip 172.31.14.190 --ps_port 1369 &> error_out_1369 &

echo "load_input_path: /mnt/efs/mnist/train_mnist1.csv 
load_input_type: csv
num_classes: 10 
num_features: 784 
limit_cols: 1000 
normalize: 1 
limit_samples: 50000000 
s3_size: 1000 
use_bias: 1 
model_type: Softmax 
minibatch_size: 20 
learning_rate: 0.000031 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1373.txt

nohup ./parameter_server --config config_1373.txt --nworkers 20 --rank 1 --ps_port 1373 &> ps_out_1373 & 

nohup ./parameter_server --config config_1373.txt --nworkers 10 --rank 2 --ps_ip 172.31.14.190 --ps_port 1373 &> error_out_1373 &

echo "load_input_path: /mnt/efs/mnist/train_mnist1.csv 
load_input_type: csv
num_classes: 10 
num_features: 784 
limit_cols: 1000 
normalize: 1 
limit_samples: 50000000 
s3_size: 1000 
use_bias: 1 
model_type: Softmax 
minibatch_size: 20 
learning_rate: 0.000031 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1377.txt

nohup ./parameter_server --config config_1377.txt --nworkers 20 --rank 1 --ps_port 1377 &> ps_out_1377 & 

nohup ./parameter_server --config config_1377.txt --nworkers 10 --rank 2 --ps_ip 172.31.14.190 --ps_port 1377 &> error_out_1377 &

echo "load_input_path: /mnt/efs/mnist/train_mnist1.csv 
load_input_type: csv
num_classes: 10 
num_features: 784 
limit_cols: 1000 
normalize: 1 
limit_samples: 50000000 
s3_size: 1000 
use_bias: 1 
model_type: Softmax 
minibatch_size: 20 
learning_rate: 0.000029 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1381.txt

nohup ./parameter_server --config config_1381.txt --nworkers 20 --rank 1 --ps_port 1381 &> ps_out_1381 & 

nohup ./parameter_server --config config_1381.txt --nworkers 10 --rank 2 --ps_ip 172.31.14.190 --ps_port 1381 &> error_out_1381 &

echo "load_input_path: /mnt/efs/mnist/train_mnist1.csv 
load_input_type: csv
num_classes: 10 
num_features: 784 
limit_cols: 1000 
normalize: 1 
limit_samples: 50000000 
s3_size: 1000 
use_bias: 1 
model_type: Softmax 
minibatch_size: 20 
learning_rate: 0.000028 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1385.txt

nohup ./parameter_server --config config_1385.txt --nworkers 20 --rank 1 --ps_port 1385 &> ps_out_1385 & 

nohup ./parameter_server --config config_1385.txt --nworkers 10 --rank 2 --ps_ip 172.31.14.190 --ps_port 1385 &> error_out_1385 &

echo "load_input_path: /mnt/efs/mnist/train_mnist1.csv 
load_input_type: csv
num_classes: 10 
num_features: 784 
limit_cols: 1000 
normalize: 1 
limit_samples: 50000000 
s3_size: 1000 
use_bias: 1 
model_type: Softmax 
minibatch_size: 20 
learning_rate: 0.000028 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1389.txt

nohup ./parameter_server --config config_1389.txt --nworkers 20 --rank 1 --ps_port 1389 &> ps_out_1389 & 

nohup ./parameter_server --config config_1389.txt --nworkers 10 --rank 2 --ps_ip 172.31.14.190 --ps_port 1389 &> error_out_1389 &

echo "load_input_path: /mnt/efs/mnist/train_mnist1.csv 
load_input_type: csv
num_classes: 10 
num_features: 784 
limit_cols: 1000 
normalize: 1 
limit_samples: 50000000 
s3_size: 1000 
use_bias: 1 
model_type: Softmax 
minibatch_size: 20 
learning_rate: 0.000026 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1393.txt

nohup ./parameter_server --config config_1393.txt --nworkers 20 --rank 1 --ps_port 1393 &> ps_out_1393 & 

nohup ./parameter_server --config config_1393.txt --nworkers 10 --rank 2 --ps_ip 172.31.14.190 --ps_port 1393 &> error_out_1393 &

echo "load_input_path: /mnt/efs/mnist/train_mnist1.csv 
load_input_type: csv
num_classes: 10 
num_features: 784 
limit_cols: 1000 
normalize: 1 
limit_samples: 50000000 
s3_size: 1000 
use_bias: 1 
model_type: Softmax 
minibatch_size: 20 
learning_rate: 0.000025 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1397.txt

nohup ./parameter_server --config config_1397.txt --nworkers 20 --rank 1 --ps_port 1397 &> ps_out_1397 & 

nohup ./parameter_server --config config_1397.txt --nworkers 10 --rank 2 --ps_ip 172.31.14.190 --ps_port 1397 &> error_out_1397 &

echo "load_input_path: /mnt/efs/mnist/train_mnist1.csv 
load_input_type: csv
num_classes: 10 
num_features: 784 
limit_cols: 1000 
normalize: 1 
limit_samples: 50000000 
s3_size: 1000 
use_bias: 1 
model_type: Softmax 
minibatch_size: 20 
learning_rate: 0.000025 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1401.txt

nohup ./parameter_server --config config_1401.txt --nworkers 20 --rank 1 --ps_port 1401 &> ps_out_1401 & 

nohup ./parameter_server --config config_1401.txt --nworkers 10 --rank 2 --ps_ip 172.31.14.190 --ps_port 1401 &> error_out_1401 &

echo "load_input_path: /mnt/efs/mnist/train_mnist1.csv 
load_input_type: csv
num_classes: 10 
num_features: 784 
limit_cols: 1000 
normalize: 1 
limit_samples: 50000000 
s3_size: 1000 
use_bias: 1 
model_type: Softmax 
minibatch_size: 20 
learning_rate: 0.000024 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1405.txt

nohup ./parameter_server --config config_1405.txt --nworkers 20 --rank 1 --ps_port 1405 &> ps_out_1405 & 

nohup ./parameter_server --config config_1405.txt --nworkers 10 --rank 2 --ps_ip 172.31.14.190 --ps_port 1405 &> error_out_1405 &

echo "load_input_path: /mnt/efs/mnist/train_mnist1.csv 
load_input_type: csv
num_classes: 10 
num_features: 784 
limit_cols: 1000 
normalize: 1 
limit_samples: 50000000 
s3_size: 1000 
use_bias: 1 
model_type: Softmax 
minibatch_size: 20 
learning_rate: 0.000023 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1409.txt

nohup ./parameter_server --config config_1409.txt --nworkers 20 --rank 1 --ps_port 1409 &> ps_out_1409 & 

nohup ./parameter_server --config config_1409.txt --nworkers 10 --rank 2 --ps_ip 172.31.14.190 --ps_port 1409 &> error_out_1409 &

echo "load_input_path: /mnt/efs/mnist/train_mnist1.csv 
load_input_type: csv
num_classes: 10 
num_features: 784 
limit_cols: 1000 
normalize: 1 
limit_samples: 50000000 
s3_size: 1000 
use_bias: 1 
model_type: Softmax 
minibatch_size: 20 
learning_rate: 0.000023 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1413.txt

nohup ./parameter_server --config config_1413.txt --nworkers 20 --rank 1 --ps_port 1413 &> ps_out_1413 & 

nohup ./parameter_server --config config_1413.txt --nworkers 10 --rank 2 --ps_ip 172.31.14.190 --ps_port 1413 &> error_out_1413 &

echo "load_input_path: /mnt/efs/mnist/train_mnist1.csv 
load_input_type: csv
num_classes: 10 
num_features: 784 
limit_cols: 1000 
normalize: 1 
limit_samples: 50000000 
s3_size: 1000 
use_bias: 1 
model_type: Softmax 
minibatch_size: 20 
learning_rate: 0.000022 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1417.txt

nohup ./parameter_server --config config_1417.txt --nworkers 20 --rank 1 --ps_port 1417 &> ps_out_1417 & 

nohup ./parameter_server --config config_1417.txt --nworkers 10 --rank 2 --ps_ip 172.31.14.190 --ps_port 1417 &> error_out_1417 &

echo "load_input_path: /mnt/efs/mnist/train_mnist1.csv 
load_input_type: csv
num_classes: 10 
num_features: 784 
limit_cols: 1000 
normalize: 1 
limit_samples: 50000000 
s3_size: 1000 
use_bias: 1 
model_type: Softmax 
minibatch_size: 20 
learning_rate: 0.000021 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1421.txt

nohup ./parameter_server --config config_1421.txt --nworkers 20 --rank 1 --ps_port 1421 &> ps_out_1421 & 

nohup ./parameter_server --config config_1421.txt --nworkers 10 --rank 2 --ps_ip 172.31.14.190 --ps_port 1421 &> error_out_1421 &

echo "load_input_path: /mnt/efs/mnist/train_mnist1.csv 
load_input_type: csv
num_classes: 10 
num_features: 784 
limit_cols: 1000 
normalize: 1 
limit_samples: 50000000 
s3_size: 1000 
use_bias: 1 
model_type: Softmax 
minibatch_size: 20 
learning_rate: 0.000021 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1425.txt

nohup ./parameter_server --config config_1425.txt --nworkers 20 --rank 1 --ps_port 1425 &> ps_out_1425 & 

nohup ./parameter_server --config config_1425.txt --nworkers 10 --rank 2 --ps_ip 172.31.14.190 --ps_port 1425 &> error_out_1425 &

echo "load_input_path: /mnt/efs/mnist/train_mnist1.csv 
load_input_type: csv
num_classes: 10 
num_features: 784 
limit_cols: 1000 
normalize: 1 
limit_samples: 50000000 
s3_size: 1000 
use_bias: 1 
model_type: Softmax 
minibatch_size: 20 
learning_rate: 0.000020 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1429.txt

nohup ./parameter_server --config config_1429.txt --nworkers 20 --rank 1 --ps_port 1429 &> ps_out_1429 & 

nohup ./parameter_server --config config_1429.txt --nworkers 10 --rank 2 --ps_ip 172.31.14.190 --ps_port 1429 &> error_out_1429 &

echo "load_input_path: /mnt/efs/mnist/train_mnist1.csv 
load_input_type: csv
num_classes: 10 
num_features: 784 
limit_cols: 1000 
normalize: 1 
limit_samples: 50000000 
s3_size: 1000 
use_bias: 1 
model_type: Softmax 
minibatch_size: 20 
learning_rate: 0.000019 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1433.txt

nohup ./parameter_server --config config_1433.txt --nworkers 20 --rank 1 --ps_port 1433 &> ps_out_1433 & 

nohup ./parameter_server --config config_1433.txt --nworkers 10 --rank 2 --ps_ip 172.31.14.190 --ps_port 1433 &> error_out_1433 &

echo "load_input_path: /mnt/efs/mnist/train_mnist1.csv 
load_input_type: csv
num_classes: 10 
num_features: 784 
limit_cols: 1000 
normalize: 1 
limit_samples: 50000000 
s3_size: 1000 
use_bias: 1 
model_type: Softmax 
minibatch_size: 20 
learning_rate: 0.000019 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1437.txt

nohup ./parameter_server --config config_1437.txt --nworkers 20 --rank 1 --ps_port 1437 &> ps_out_1437 & 

nohup ./parameter_server --config config_1437.txt --nworkers 10 --rank 2 --ps_ip 172.31.14.190 --ps_port 1437 &> error_out_1437 &

echo "load_input_path: /mnt/efs/mnist/train_mnist1.csv 
load_input_type: csv
num_classes: 10 
num_features: 784 
limit_cols: 1000 
normalize: 1 
limit_samples: 50000000 
s3_size: 1000 
use_bias: 1 
model_type: Softmax 
minibatch_size: 20 
learning_rate: 0.000019 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1441.txt

nohup ./parameter_server --config config_1441.txt --nworkers 20 --rank 1 --ps_port 1441 &> ps_out_1441 & 

nohup ./parameter_server --config config_1441.txt --nworkers 10 --rank 2 --ps_ip 172.31.14.190 --ps_port 1441 &> error_out_1441 &

echo "load_input_path: /mnt/efs/mnist/train_mnist1.csv 
load_input_type: csv
num_classes: 10 
num_features: 784 
limit_cols: 1000 
normalize: 1 
limit_samples: 50000000 
s3_size: 1000 
use_bias: 1 
model_type: Softmax 
minibatch_size: 20 
learning_rate: 0.000018 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1445.txt

nohup ./parameter_server --config config_1445.txt --nworkers 20 --rank 1 --ps_port 1445 &> ps_out_1445 & 

nohup ./parameter_server --config config_1445.txt --nworkers 10 --rank 2 --ps_ip 172.31.14.190 --ps_port 1445 &> error_out_1445 &

echo "load_input_path: /mnt/efs/mnist/train_mnist1.csv 
load_input_type: csv
num_classes: 10 
num_features: 784 
limit_cols: 1000 
normalize: 1 
limit_samples: 50000000 
s3_size: 1000 
use_bias: 1 
model_type: Softmax 
minibatch_size: 20 
learning_rate: 0.000018 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1449.txt

nohup ./parameter_server --config config_1449.txt --nworkers 20 --rank 1 --ps_port 1449 &> ps_out_1449 & 

nohup ./parameter_server --config config_1449.txt --nworkers 10 --rank 2 --ps_ip 172.31.14.190 --ps_port 1449 &> error_out_1449 &

echo "load_input_path: /mnt/efs/mnist/train_mnist1.csv 
load_input_type: csv
num_classes: 10 
num_features: 784 
limit_cols: 1000 
normalize: 1 
limit_samples: 50000000 
s3_size: 1000 
use_bias: 1 
model_type: Softmax 
minibatch_size: 20 
learning_rate: 0.000017 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1453.txt

nohup ./parameter_server --config config_1453.txt --nworkers 20 --rank 1 --ps_port 1453 &> ps_out_1453 & 

nohup ./parameter_server --config config_1453.txt --nworkers 10 --rank 2 --ps_ip 172.31.14.190 --ps_port 1453 &> error_out_1453 &

echo "load_input_path: /mnt/efs/mnist/train_mnist1.csv 
load_input_type: csv
num_classes: 10 
num_features: 784 
limit_cols: 1000 
normalize: 1 
limit_samples: 50000000 
s3_size: 1000 
use_bias: 1 
model_type: Softmax 
minibatch_size: 20 
learning_rate: 0.000017 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1457.txt

nohup ./parameter_server --config config_1457.txt --nworkers 20 --rank 1 --ps_port 1457 &> ps_out_1457 & 

nohup ./parameter_server --config config_1457.txt --nworkers 10 --rank 2 --ps_ip 172.31.14.190 --ps_port 1457 &> error_out_1457 &

echo "load_input_path: /mnt/efs/mnist/train_mnist1.csv 
load_input_type: csv
num_classes: 10 
num_features: 784 
limit_cols: 1000 
normalize: 1 
limit_samples: 50000000 
s3_size: 1000 
use_bias: 1 
model_type: Softmax 
minibatch_size: 20 
learning_rate: 0.000017 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1461.txt

nohup ./parameter_server --config config_1461.txt --nworkers 20 --rank 1 --ps_port 1461 &> ps_out_1461 & 

nohup ./parameter_server --config config_1461.txt --nworkers 10 --rank 2 --ps_ip 172.31.14.190 --ps_port 1461 &> error_out_1461 &

