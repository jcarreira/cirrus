echo "input_path: /mnt/efs/mnist/train_mnist1.csv 
input_type: csv
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
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1339.txt

nohup ./parameter_server --config config_1339.txt --nworkers 20 --rank 1 --ps_port 1339 &> ps_out_1339 & 

nohup ./parameter_server --config config_1339.txt --nworkers 10 --rank 2 --ps_ip 172.31.6.212 --ps_port 1339 &> error_out_1339 &

echo "input_path: /mnt/efs/mnist/train_mnist1.csv 
input_type: csv
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
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1343.txt

nohup ./parameter_server --config config_1343.txt --nworkers 20 --rank 1 --ps_port 1343 &> ps_out_1343 & 

nohup ./parameter_server --config config_1343.txt --nworkers 10 --rank 2 --ps_ip 172.31.6.212 --ps_port 1343 &> error_out_1343 &

echo "input_path: /mnt/efs/mnist/train_mnist1.csv 
input_type: csv
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
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1347.txt

nohup ./parameter_server --config config_1347.txt --nworkers 20 --rank 1 --ps_port 1347 &> ps_out_1347 & 

nohup ./parameter_server --config config_1347.txt --nworkers 10 --rank 2 --ps_ip 172.31.6.212 --ps_port 1347 &> error_out_1347 &

echo "input_path: /mnt/efs/mnist/train_mnist1.csv 
input_type: csv
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
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1351.txt

nohup ./parameter_server --config config_1351.txt --nworkers 20 --rank 1 --ps_port 1351 &> ps_out_1351 & 

nohup ./parameter_server --config config_1351.txt --nworkers 10 --rank 2 --ps_ip 172.31.6.212 --ps_port 1351 &> error_out_1351 &

echo "input_path: /mnt/efs/mnist/train_mnist1.csv 
input_type: csv
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
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1355.txt

nohup ./parameter_server --config config_1355.txt --nworkers 20 --rank 1 --ps_port 1355 &> ps_out_1355 & 

nohup ./parameter_server --config config_1355.txt --nworkers 10 --rank 2 --ps_ip 172.31.6.212 --ps_port 1355 &> error_out_1355 &

echo "input_path: /mnt/efs/mnist/train_mnist1.csv 
input_type: csv
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
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1359.txt

nohup ./parameter_server --config config_1359.txt --nworkers 20 --rank 1 --ps_port 1359 &> ps_out_1359 & 

nohup ./parameter_server --config config_1359.txt --nworkers 10 --rank 2 --ps_ip 172.31.6.212 --ps_port 1359 &> error_out_1359 &

echo "input_path: /mnt/efs/mnist/train_mnist1.csv 
input_type: csv
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
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1363.txt

nohup ./parameter_server --config config_1363.txt --nworkers 20 --rank 1 --ps_port 1363 &> ps_out_1363 & 

nohup ./parameter_server --config config_1363.txt --nworkers 10 --rank 2 --ps_ip 172.31.6.212 --ps_port 1363 &> error_out_1363 &

echo "input_path: /mnt/efs/mnist/train_mnist1.csv 
input_type: csv
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
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1367.txt

nohup ./parameter_server --config config_1367.txt --nworkers 20 --rank 1 --ps_port 1367 &> ps_out_1367 & 

nohup ./parameter_server --config config_1367.txt --nworkers 10 --rank 2 --ps_ip 172.31.6.212 --ps_port 1367 &> error_out_1367 &

echo "input_path: /mnt/efs/mnist/train_mnist1.csv 
input_type: csv
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
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1371.txt

nohup ./parameter_server --config config_1371.txt --nworkers 20 --rank 1 --ps_port 1371 &> ps_out_1371 & 

nohup ./parameter_server --config config_1371.txt --nworkers 10 --rank 2 --ps_ip 172.31.6.212 --ps_port 1371 &> error_out_1371 &

echo "input_path: /mnt/efs/mnist/train_mnist1.csv 
input_type: csv
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
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1375.txt

nohup ./parameter_server --config config_1375.txt --nworkers 20 --rank 1 --ps_port 1375 &> ps_out_1375 & 

nohup ./parameter_server --config config_1375.txt --nworkers 10 --rank 2 --ps_ip 172.31.6.212 --ps_port 1375 &> error_out_1375 &

echo "input_path: /mnt/efs/mnist/train_mnist1.csv 
input_type: csv
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
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1379.txt

nohup ./parameter_server --config config_1379.txt --nworkers 20 --rank 1 --ps_port 1379 &> ps_out_1379 & 

nohup ./parameter_server --config config_1379.txt --nworkers 10 --rank 2 --ps_ip 172.31.6.212 --ps_port 1379 &> error_out_1379 &

echo "input_path: /mnt/efs/mnist/train_mnist1.csv 
input_type: csv
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
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1383.txt

nohup ./parameter_server --config config_1383.txt --nworkers 20 --rank 1 --ps_port 1383 &> ps_out_1383 & 

nohup ./parameter_server --config config_1383.txt --nworkers 10 --rank 2 --ps_ip 172.31.6.212 --ps_port 1383 &> error_out_1383 &

echo "input_path: /mnt/efs/mnist/train_mnist1.csv 
input_type: csv
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
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1387.txt

nohup ./parameter_server --config config_1387.txt --nworkers 20 --rank 1 --ps_port 1387 &> ps_out_1387 & 

nohup ./parameter_server --config config_1387.txt --nworkers 10 --rank 2 --ps_ip 172.31.6.212 --ps_port 1387 &> error_out_1387 &

echo "input_path: /mnt/efs/mnist/train_mnist1.csv 
input_type: csv
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
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1391.txt

nohup ./parameter_server --config config_1391.txt --nworkers 20 --rank 1 --ps_port 1391 &> ps_out_1391 & 

nohup ./parameter_server --config config_1391.txt --nworkers 10 --rank 2 --ps_ip 172.31.6.212 --ps_port 1391 &> error_out_1391 &

echo "input_path: /mnt/efs/mnist/train_mnist1.csv 
input_type: csv
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
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1395.txt

nohup ./parameter_server --config config_1395.txt --nworkers 20 --rank 1 --ps_port 1395 &> ps_out_1395 & 

nohup ./parameter_server --config config_1395.txt --nworkers 10 --rank 2 --ps_ip 172.31.6.212 --ps_port 1395 &> error_out_1395 &

echo "input_path: /mnt/efs/mnist/train_mnist1.csv 
input_type: csv
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
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1399.txt

nohup ./parameter_server --config config_1399.txt --nworkers 20 --rank 1 --ps_port 1399 &> ps_out_1399 & 

nohup ./parameter_server --config config_1399.txt --nworkers 10 --rank 2 --ps_ip 172.31.6.212 --ps_port 1399 &> error_out_1399 &

echo "input_path: /mnt/efs/mnist/train_mnist1.csv 
input_type: csv
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
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1403.txt

nohup ./parameter_server --config config_1403.txt --nworkers 20 --rank 1 --ps_port 1403 &> ps_out_1403 & 

nohup ./parameter_server --config config_1403.txt --nworkers 10 --rank 2 --ps_ip 172.31.6.212 --ps_port 1403 &> error_out_1403 &

echo "input_path: /mnt/efs/mnist/train_mnist1.csv 
input_type: csv
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
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1407.txt

nohup ./parameter_server --config config_1407.txt --nworkers 20 --rank 1 --ps_port 1407 &> ps_out_1407 & 

nohup ./parameter_server --config config_1407.txt --nworkers 10 --rank 2 --ps_ip 172.31.6.212 --ps_port 1407 &> error_out_1407 &

echo "input_path: /mnt/efs/mnist/train_mnist1.csv 
input_type: csv
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
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1411.txt

nohup ./parameter_server --config config_1411.txt --nworkers 20 --rank 1 --ps_port 1411 &> ps_out_1411 & 

nohup ./parameter_server --config config_1411.txt --nworkers 10 --rank 2 --ps_ip 172.31.6.212 --ps_port 1411 &> error_out_1411 &

echo "input_path: /mnt/efs/mnist/train_mnist1.csv 
input_type: csv
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
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1415.txt

nohup ./parameter_server --config config_1415.txt --nworkers 20 --rank 1 --ps_port 1415 &> ps_out_1415 & 

nohup ./parameter_server --config config_1415.txt --nworkers 10 --rank 2 --ps_ip 172.31.6.212 --ps_port 1415 &> error_out_1415 &

echo "input_path: /mnt/efs/mnist/train_mnist1.csv 
input_type: csv
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
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1419.txt

nohup ./parameter_server --config config_1419.txt --nworkers 20 --rank 1 --ps_port 1419 &> ps_out_1419 & 

nohup ./parameter_server --config config_1419.txt --nworkers 10 --rank 2 --ps_ip 172.31.6.212 --ps_port 1419 &> error_out_1419 &

echo "input_path: /mnt/efs/mnist/train_mnist1.csv 
input_type: csv
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
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1423.txt

nohup ./parameter_server --config config_1423.txt --nworkers 20 --rank 1 --ps_port 1423 &> ps_out_1423 & 

nohup ./parameter_server --config config_1423.txt --nworkers 10 --rank 2 --ps_ip 172.31.6.212 --ps_port 1423 &> error_out_1423 &

echo "input_path: /mnt/efs/mnist/train_mnist1.csv 
input_type: csv
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
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1427.txt

nohup ./parameter_server --config config_1427.txt --nworkers 20 --rank 1 --ps_port 1427 &> ps_out_1427 & 

nohup ./parameter_server --config config_1427.txt --nworkers 10 --rank 2 --ps_ip 172.31.6.212 --ps_port 1427 &> error_out_1427 &

echo "input_path: /mnt/efs/mnist/train_mnist1.csv 
input_type: csv
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
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1431.txt

nohup ./parameter_server --config config_1431.txt --nworkers 20 --rank 1 --ps_port 1431 &> ps_out_1431 & 

nohup ./parameter_server --config config_1431.txt --nworkers 10 --rank 2 --ps_ip 172.31.6.212 --ps_port 1431 &> error_out_1431 &

echo "input_path: /mnt/efs/mnist/train_mnist1.csv 
input_type: csv
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
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1435.txt

nohup ./parameter_server --config config_1435.txt --nworkers 20 --rank 1 --ps_port 1435 &> ps_out_1435 & 

nohup ./parameter_server --config config_1435.txt --nworkers 10 --rank 2 --ps_ip 172.31.6.212 --ps_port 1435 &> error_out_1435 &

echo "input_path: /mnt/efs/mnist/train_mnist1.csv 
input_type: csv
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
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1439.txt

nohup ./parameter_server --config config_1439.txt --nworkers 20 --rank 1 --ps_port 1439 &> ps_out_1439 & 

nohup ./parameter_server --config config_1439.txt --nworkers 10 --rank 2 --ps_ip 172.31.6.212 --ps_port 1439 &> error_out_1439 &

echo "input_path: /mnt/efs/mnist/train_mnist1.csv 
input_type: csv
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
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1443.txt

nohup ./parameter_server --config config_1443.txt --nworkers 20 --rank 1 --ps_port 1443 &> ps_out_1443 & 

nohup ./parameter_server --config config_1443.txt --nworkers 10 --rank 2 --ps_ip 172.31.6.212 --ps_port 1443 &> error_out_1443 &

echo "input_path: /mnt/efs/mnist/train_mnist1.csv 
input_type: csv
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
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1447.txt

nohup ./parameter_server --config config_1447.txt --nworkers 20 --rank 1 --ps_port 1447 &> ps_out_1447 & 

nohup ./parameter_server --config config_1447.txt --nworkers 10 --rank 2 --ps_ip 172.31.6.212 --ps_port 1447 &> error_out_1447 &

echo "input_path: /mnt/efs/mnist/train_mnist1.csv 
input_type: csv
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
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1451.txt

nohup ./parameter_server --config config_1451.txt --nworkers 20 --rank 1 --ps_port 1451 &> ps_out_1451 & 

nohup ./parameter_server --config config_1451.txt --nworkers 10 --rank 2 --ps_ip 172.31.6.212 --ps_port 1451 &> error_out_1451 &

echo "input_path: /mnt/efs/mnist/train_mnist1.csv 
input_type: csv
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
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1455.txt

nohup ./parameter_server --config config_1455.txt --nworkers 20 --rank 1 --ps_port 1455 &> ps_out_1455 & 

nohup ./parameter_server --config config_1455.txt --nworkers 10 --rank 2 --ps_ip 172.31.6.212 --ps_port 1455 &> error_out_1455 &

echo "input_path: /mnt/efs/mnist/train_mnist1.csv 
input_type: csv
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
s3_bucket: cirrus-mnist 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-20 
test_set: 21-42" > config_1459.txt

nohup ./parameter_server --config config_1459.txt --nworkers 20 --rank 1 --ps_port 1459 &> ps_out_1459 & 

nohup ./parameter_server --config config_1459.txt --nworkers 10 --rank 2 --ps_ip 172.31.6.212 --ps_port 1459 &> error_out_1459 &

