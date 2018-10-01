echo "load_input_path: /mnt/efs/criteo_kaggle/train.csv 
load_input_type: csv
num_classes: 2 
num_features: 13 
limit_cols: 14 
normalize: 1 
limit_samples: 50000000 
s3_size: 50000 
use_bias: 1 
model_type: LogisticRegression 
minibatch_size: 20 
learning_rate: 0.100000 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: criteo-kaggle-19b 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-824 
test_set: 835-840" > config_1339.txt

nohup ./parameter_server --config config_1339.txt --nworkers 40 --rank 1 --ps_port 1339 &> ps_out_1339 & 

nohup ./parameter_server --config config_1339.txt --nworkers 20 --rank 2 --ps_ip 172.31.10.106 --ps_port 1339 &> error_out_1339 &

echo "load_input_path: /mnt/efs/criteo_kaggle/train.csv 
load_input_type: csv
num_classes: 2 
num_features: 13 
limit_cols: 14 
normalize: 1 
limit_samples: 50000000 
s3_size: 50000 
use_bias: 1 
model_type: LogisticRegression 
minibatch_size: 20 
learning_rate: 0.050000 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: criteo-kaggle-19b 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-824 
test_set: 835-840" > config_1343.txt

nohup ./parameter_server --config config_1343.txt --nworkers 40 --rank 1 --ps_port 1343 &> ps_out_1343 & 

nohup ./parameter_server --config config_1343.txt --nworkers 20 --rank 2 --ps_ip 172.31.10.106 --ps_port 1343 &> error_out_1343 &

echo "load_input_path: /mnt/efs/criteo_kaggle/train.csv 
load_input_type: csv
num_classes: 2 
num_features: 13 
limit_cols: 14 
normalize: 1 
limit_samples: 50000000 
s3_size: 50000 
use_bias: 1 
model_type: LogisticRegression 
minibatch_size: 20 
learning_rate: 0.050000 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: criteo-kaggle-19b 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-824 
test_set: 835-840" > config_1347.txt

nohup ./parameter_server --config config_1347.txt --nworkers 40 --rank 1 --ps_port 1347 &> ps_out_1347 & 

nohup ./parameter_server --config config_1347.txt --nworkers 20 --rank 2 --ps_ip 172.31.10.106 --ps_port 1347 &> error_out_1347 &

echo "load_input_path: /mnt/efs/criteo_kaggle/train.csv 
load_input_type: csv
num_classes: 2 
num_features: 13 
limit_cols: 14 
normalize: 1 
limit_samples: 50000000 
s3_size: 50000 
use_bias: 1 
model_type: LogisticRegression 
minibatch_size: 20 
learning_rate: 0.033333 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: criteo-kaggle-19b 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-824 
test_set: 835-840" > config_1351.txt

nohup ./parameter_server --config config_1351.txt --nworkers 40 --rank 1 --ps_port 1351 &> ps_out_1351 & 

nohup ./parameter_server --config config_1351.txt --nworkers 20 --rank 2 --ps_ip 172.31.10.106 --ps_port 1351 &> error_out_1351 &

echo "load_input_path: /mnt/efs/criteo_kaggle/train.csv 
load_input_type: csv
num_classes: 2 
num_features: 13 
limit_cols: 14 
normalize: 1 
limit_samples: 50000000 
s3_size: 50000 
use_bias: 1 
model_type: LogisticRegression 
minibatch_size: 20 
learning_rate: 0.025000 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: criteo-kaggle-19b 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-824 
test_set: 835-840" > config_1355.txt

nohup ./parameter_server --config config_1355.txt --nworkers 40 --rank 1 --ps_port 1355 &> ps_out_1355 & 

nohup ./parameter_server --config config_1355.txt --nworkers 20 --rank 2 --ps_ip 172.31.10.106 --ps_port 1355 &> error_out_1355 &

echo "load_input_path: /mnt/efs/criteo_kaggle/train.csv 
load_input_type: csv
num_classes: 2 
num_features: 13 
limit_cols: 14 
normalize: 1 
limit_samples: 50000000 
s3_size: 50000 
use_bias: 1 
model_type: LogisticRegression 
minibatch_size: 20 
learning_rate: 0.025000 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: criteo-kaggle-19b 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-824 
test_set: 835-840" > config_1359.txt

nohup ./parameter_server --config config_1359.txt --nworkers 40 --rank 1 --ps_port 1359 &> ps_out_1359 & 

nohup ./parameter_server --config config_1359.txt --nworkers 20 --rank 2 --ps_ip 172.31.10.106 --ps_port 1359 &> error_out_1359 &

echo "load_input_path: /mnt/efs/criteo_kaggle/train.csv 
load_input_type: csv
num_classes: 2 
num_features: 13 
limit_cols: 14 
normalize: 1 
limit_samples: 50000000 
s3_size: 50000 
use_bias: 1 
model_type: LogisticRegression 
minibatch_size: 20 
learning_rate: 0.020000 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: criteo-kaggle-19b 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-824 
test_set: 835-840" > config_1363.txt

nohup ./parameter_server --config config_1363.txt --nworkers 40 --rank 1 --ps_port 1363 &> ps_out_1363 & 

nohup ./parameter_server --config config_1363.txt --nworkers 20 --rank 2 --ps_ip 172.31.10.106 --ps_port 1363 &> error_out_1363 &

echo "load_input_path: /mnt/efs/criteo_kaggle/train.csv 
load_input_type: csv
num_classes: 2 
num_features: 13 
limit_cols: 14 
normalize: 1 
limit_samples: 50000000 
s3_size: 50000 
use_bias: 1 
model_type: LogisticRegression 
minibatch_size: 20 
learning_rate: 0.016667 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: criteo-kaggle-19b 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-824 
test_set: 835-840" > config_1367.txt

nohup ./parameter_server --config config_1367.txt --nworkers 40 --rank 1 --ps_port 1367 &> ps_out_1367 & 

nohup ./parameter_server --config config_1367.txt --nworkers 20 --rank 2 --ps_ip 172.31.10.106 --ps_port 1367 &> error_out_1367 &

echo "load_input_path: /mnt/efs/criteo_kaggle/train.csv 
load_input_type: csv
num_classes: 2 
num_features: 13 
limit_cols: 14 
normalize: 1 
limit_samples: 50000000 
s3_size: 50000 
use_bias: 1 
model_type: LogisticRegression 
minibatch_size: 20 
learning_rate: 0.016667 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: criteo-kaggle-19b 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-824 
test_set: 835-840" > config_1371.txt

nohup ./parameter_server --config config_1371.txt --nworkers 40 --rank 1 --ps_port 1371 &> ps_out_1371 & 

nohup ./parameter_server --config config_1371.txt --nworkers 20 --rank 2 --ps_ip 172.31.10.106 --ps_port 1371 &> error_out_1371 &

echo "load_input_path: /mnt/efs/criteo_kaggle/train.csv 
load_input_type: csv
num_classes: 2 
num_features: 13 
limit_cols: 14 
normalize: 1 
limit_samples: 50000000 
s3_size: 50000 
use_bias: 1 
model_type: LogisticRegression 
minibatch_size: 20 
learning_rate: 0.014286 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: criteo-kaggle-19b 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-824 
test_set: 835-840" > config_1375.txt

nohup ./parameter_server --config config_1375.txt --nworkers 40 --rank 1 --ps_port 1375 &> ps_out_1375 & 

nohup ./parameter_server --config config_1375.txt --nworkers 20 --rank 2 --ps_ip 172.31.10.106 --ps_port 1375 &> error_out_1375 &

echo "load_input_path: /mnt/efs/criteo_kaggle/train.csv 
load_input_type: csv
num_classes: 2 
num_features: 13 
limit_cols: 14 
normalize: 1 
limit_samples: 50000000 
s3_size: 50000 
use_bias: 1 
model_type: LogisticRegression 
minibatch_size: 20 
learning_rate: 0.012500 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: criteo-kaggle-19b 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-824 
test_set: 835-840" > config_1379.txt

nohup ./parameter_server --config config_1379.txt --nworkers 40 --rank 1 --ps_port 1379 &> ps_out_1379 & 

nohup ./parameter_server --config config_1379.txt --nworkers 20 --rank 2 --ps_ip 172.31.10.106 --ps_port 1379 &> error_out_1379 &

echo "load_input_path: /mnt/efs/criteo_kaggle/train.csv 
load_input_type: csv
num_classes: 2 
num_features: 13 
limit_cols: 14 
normalize: 1 
limit_samples: 50000000 
s3_size: 50000 
use_bias: 1 
model_type: LogisticRegression 
minibatch_size: 20 
learning_rate: 0.012500 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: criteo-kaggle-19b 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-824 
test_set: 835-840" > config_1383.txt

nohup ./parameter_server --config config_1383.txt --nworkers 40 --rank 1 --ps_port 1383 &> ps_out_1383 & 

nohup ./parameter_server --config config_1383.txt --nworkers 20 --rank 2 --ps_ip 172.31.10.106 --ps_port 1383 &> error_out_1383 &

echo "load_input_path: /mnt/efs/criteo_kaggle/train.csv 
load_input_type: csv
num_classes: 2 
num_features: 13 
limit_cols: 14 
normalize: 1 
limit_samples: 50000000 
s3_size: 50000 
use_bias: 1 
model_type: LogisticRegression 
minibatch_size: 20 
learning_rate: 0.011111 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: criteo-kaggle-19b 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-824 
test_set: 835-840" > config_1387.txt

nohup ./parameter_server --config config_1387.txt --nworkers 40 --rank 1 --ps_port 1387 &> ps_out_1387 & 

nohup ./parameter_server --config config_1387.txt --nworkers 20 --rank 2 --ps_ip 172.31.10.106 --ps_port 1387 &> error_out_1387 &

echo "load_input_path: /mnt/efs/criteo_kaggle/train.csv 
load_input_type: csv
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
dataset_format: binary 
s3_bucket: criteo-kaggle-19b 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-824 
test_set: 835-840" > config_1391.txt

nohup ./parameter_server --config config_1391.txt --nworkers 40 --rank 1 --ps_port 1391 &> ps_out_1391 & 

nohup ./parameter_server --config config_1391.txt --nworkers 20 --rank 2 --ps_ip 172.31.10.106 --ps_port 1391 &> error_out_1391 &

echo "load_input_path: /mnt/efs/criteo_kaggle/train.csv 
load_input_type: csv
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
dataset_format: binary 
s3_bucket: criteo-kaggle-19b 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-824 
test_set: 835-840" > config_1395.txt

nohup ./parameter_server --config config_1395.txt --nworkers 40 --rank 1 --ps_port 1395 &> ps_out_1395 & 

nohup ./parameter_server --config config_1395.txt --nworkers 20 --rank 2 --ps_ip 172.31.10.106 --ps_port 1395 &> error_out_1395 &

echo "load_input_path: /mnt/efs/criteo_kaggle/train.csv 
load_input_type: csv
num_classes: 2 
num_features: 13 
limit_cols: 14 
normalize: 1 
limit_samples: 50000000 
s3_size: 50000 
use_bias: 1 
model_type: LogisticRegression 
minibatch_size: 20 
learning_rate: 0.009091 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: criteo-kaggle-19b 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-824 
test_set: 835-840" > config_1399.txt

nohup ./parameter_server --config config_1399.txt --nworkers 40 --rank 1 --ps_port 1399 &> ps_out_1399 & 

nohup ./parameter_server --config config_1399.txt --nworkers 20 --rank 2 --ps_ip 172.31.10.106 --ps_port 1399 &> error_out_1399 &

echo "load_input_path: /mnt/efs/criteo_kaggle/train.csv 
load_input_type: csv
num_classes: 2 
num_features: 13 
limit_cols: 14 
normalize: 1 
limit_samples: 50000000 
s3_size: 50000 
use_bias: 1 
model_type: LogisticRegression 
minibatch_size: 20 
learning_rate: 0.008333 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: criteo-kaggle-19b 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-824 
test_set: 835-840" > config_1403.txt

nohup ./parameter_server --config config_1403.txt --nworkers 40 --rank 1 --ps_port 1403 &> ps_out_1403 & 

nohup ./parameter_server --config config_1403.txt --nworkers 20 --rank 2 --ps_ip 172.31.10.106 --ps_port 1403 &> error_out_1403 &

echo "load_input_path: /mnt/efs/criteo_kaggle/train.csv 
load_input_type: csv
num_classes: 2 
num_features: 13 
limit_cols: 14 
normalize: 1 
limit_samples: 50000000 
s3_size: 50000 
use_bias: 1 
model_type: LogisticRegression 
minibatch_size: 20 
learning_rate: 0.008333 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: criteo-kaggle-19b 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-824 
test_set: 835-840" > config_1407.txt

nohup ./parameter_server --config config_1407.txt --nworkers 40 --rank 1 --ps_port 1407 &> ps_out_1407 & 

nohup ./parameter_server --config config_1407.txt --nworkers 20 --rank 2 --ps_ip 172.31.10.106 --ps_port 1407 &> error_out_1407 &

echo "load_input_path: /mnt/efs/criteo_kaggle/train.csv 
load_input_type: csv
num_classes: 2 
num_features: 13 
limit_cols: 14 
normalize: 1 
limit_samples: 50000000 
s3_size: 50000 
use_bias: 1 
model_type: LogisticRegression 
minibatch_size: 20 
learning_rate: 0.007692 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: criteo-kaggle-19b 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-824 
test_set: 835-840" > config_1411.txt

nohup ./parameter_server --config config_1411.txt --nworkers 40 --rank 1 --ps_port 1411 &> ps_out_1411 & 

nohup ./parameter_server --config config_1411.txt --nworkers 20 --rank 2 --ps_ip 172.31.10.106 --ps_port 1411 &> error_out_1411 &

echo "load_input_path: /mnt/efs/criteo_kaggle/train.csv 
load_input_type: csv
num_classes: 2 
num_features: 13 
limit_cols: 14 
normalize: 1 
limit_samples: 50000000 
s3_size: 50000 
use_bias: 1 
model_type: LogisticRegression 
minibatch_size: 20 
learning_rate: 0.007143 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: criteo-kaggle-19b 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-824 
test_set: 835-840" > config_1415.txt

nohup ./parameter_server --config config_1415.txt --nworkers 40 --rank 1 --ps_port 1415 &> ps_out_1415 & 

nohup ./parameter_server --config config_1415.txt --nworkers 20 --rank 2 --ps_ip 172.31.10.106 --ps_port 1415 &> error_out_1415 &

echo "load_input_path: /mnt/efs/criteo_kaggle/train.csv 
load_input_type: csv
num_classes: 2 
num_features: 13 
limit_cols: 14 
normalize: 1 
limit_samples: 50000000 
s3_size: 50000 
use_bias: 1 
model_type: LogisticRegression 
minibatch_size: 20 
learning_rate: 0.007143 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: criteo-kaggle-19b 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-824 
test_set: 835-840" > config_1419.txt

nohup ./parameter_server --config config_1419.txt --nworkers 40 --rank 1 --ps_port 1419 &> ps_out_1419 & 

nohup ./parameter_server --config config_1419.txt --nworkers 20 --rank 2 --ps_ip 172.31.10.106 --ps_port 1419 &> error_out_1419 &

echo "load_input_path: /mnt/efs/criteo_kaggle/train.csv 
load_input_type: csv
num_classes: 2 
num_features: 13 
limit_cols: 14 
normalize: 1 
limit_samples: 50000000 
s3_size: 50000 
use_bias: 1 
model_type: LogisticRegression 
minibatch_size: 20 
learning_rate: 0.006667 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: criteo-kaggle-19b 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-824 
test_set: 835-840" > config_1423.txt

nohup ./parameter_server --config config_1423.txt --nworkers 40 --rank 1 --ps_port 1423 &> ps_out_1423 & 

nohup ./parameter_server --config config_1423.txt --nworkers 20 --rank 2 --ps_ip 172.31.10.106 --ps_port 1423 &> error_out_1423 &

echo "load_input_path: /mnt/efs/criteo_kaggle/train.csv 
load_input_type: csv
num_classes: 2 
num_features: 13 
limit_cols: 14 
normalize: 1 
limit_samples: 50000000 
s3_size: 50000 
use_bias: 1 
model_type: LogisticRegression 
minibatch_size: 20 
learning_rate: 0.006250 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: criteo-kaggle-19b 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-824 
test_set: 835-840" > config_1427.txt

nohup ./parameter_server --config config_1427.txt --nworkers 40 --rank 1 --ps_port 1427 &> ps_out_1427 & 

nohup ./parameter_server --config config_1427.txt --nworkers 20 --rank 2 --ps_ip 172.31.10.106 --ps_port 1427 &> error_out_1427 &

echo "load_input_path: /mnt/efs/criteo_kaggle/train.csv 
load_input_type: csv
num_classes: 2 
num_features: 13 
limit_cols: 14 
normalize: 1 
limit_samples: 50000000 
s3_size: 50000 
use_bias: 1 
model_type: LogisticRegression 
minibatch_size: 20 
learning_rate: 0.006250 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: criteo-kaggle-19b 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-824 
test_set: 835-840" > config_1431.txt

nohup ./parameter_server --config config_1431.txt --nworkers 40 --rank 1 --ps_port 1431 &> ps_out_1431 & 

nohup ./parameter_server --config config_1431.txt --nworkers 20 --rank 2 --ps_ip 172.31.10.106 --ps_port 1431 &> error_out_1431 &

echo "load_input_path: /mnt/efs/criteo_kaggle/train.csv 
load_input_type: csv
num_classes: 2 
num_features: 13 
limit_cols: 14 
normalize: 1 
limit_samples: 50000000 
s3_size: 50000 
use_bias: 1 
model_type: LogisticRegression 
minibatch_size: 20 
learning_rate: 0.005882 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: criteo-kaggle-19b 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-824 
test_set: 835-840" > config_1435.txt

nohup ./parameter_server --config config_1435.txt --nworkers 40 --rank 1 --ps_port 1435 &> ps_out_1435 & 

nohup ./parameter_server --config config_1435.txt --nworkers 20 --rank 2 --ps_ip 172.31.10.106 --ps_port 1435 &> error_out_1435 &

echo "load_input_path: /mnt/efs/criteo_kaggle/train.csv 
load_input_type: csv
num_classes: 2 
num_features: 13 
limit_cols: 14 
normalize: 1 
limit_samples: 50000000 
s3_size: 50000 
use_bias: 1 
model_type: LogisticRegression 
minibatch_size: 20 
learning_rate: 0.005556 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: criteo-kaggle-19b 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-824 
test_set: 835-840" > config_1439.txt

nohup ./parameter_server --config config_1439.txt --nworkers 40 --rank 1 --ps_port 1439 &> ps_out_1439 & 

nohup ./parameter_server --config config_1439.txt --nworkers 20 --rank 2 --ps_ip 172.31.10.106 --ps_port 1439 &> error_out_1439 &

echo "load_input_path: /mnt/efs/criteo_kaggle/train.csv 
load_input_type: csv
num_classes: 2 
num_features: 13 
limit_cols: 14 
normalize: 1 
limit_samples: 50000000 
s3_size: 50000 
use_bias: 1 
model_type: LogisticRegression 
minibatch_size: 20 
learning_rate: 0.005556 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: criteo-kaggle-19b 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-824 
test_set: 835-840" > config_1443.txt

nohup ./parameter_server --config config_1443.txt --nworkers 40 --rank 1 --ps_port 1443 &> ps_out_1443 & 

nohup ./parameter_server --config config_1443.txt --nworkers 20 --rank 2 --ps_ip 172.31.10.106 --ps_port 1443 &> error_out_1443 &

echo "load_input_path: /mnt/efs/criteo_kaggle/train.csv 
load_input_type: csv
num_classes: 2 
num_features: 13 
limit_cols: 14 
normalize: 1 
limit_samples: 50000000 
s3_size: 50000 
use_bias: 1 
model_type: LogisticRegression 
minibatch_size: 20 
learning_rate: 0.005263 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: criteo-kaggle-19b 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-824 
test_set: 835-840" > config_1447.txt

nohup ./parameter_server --config config_1447.txt --nworkers 40 --rank 1 --ps_port 1447 &> ps_out_1447 & 

nohup ./parameter_server --config config_1447.txt --nworkers 20 --rank 2 --ps_ip 172.31.10.106 --ps_port 1447 &> error_out_1447 &

