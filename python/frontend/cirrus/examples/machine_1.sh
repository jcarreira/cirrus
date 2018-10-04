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
train_set: 10-19 
test_set: 0-9" > python_files/config_1339.txt

nohup ./parameter_server --config python_files/config_1339.txt --nworkers 10 --rank 1 --ps_port 1339 &> python_files/ps_out_1339 & 

nohup ./parameter_server --config python_files/config_1339.txt --nworkers 5 --rank 2 --ps_ip 172.31.5.74 --ps_port 1339 &> python_files/error_out_1339 &

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
train_set: 10-19 
test_set: 0-9" > python_files/config_1343.txt

nohup ./parameter_server --config python_files/config_1343.txt --nworkers 10 --rank 1 --ps_port 1343 &> python_files/ps_out_1343 & 

nohup ./parameter_server --config python_files/config_1343.txt --nworkers 5 --rank 2 --ps_ip 172.31.5.74 --ps_port 1343 &> python_files/error_out_1343 &

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
train_set: 10-19 
test_set: 0-9" > python_files/config_1347.txt

nohup ./parameter_server --config python_files/config_1347.txt --nworkers 10 --rank 1 --ps_port 1347 &> python_files/ps_out_1347 & 

nohup ./parameter_server --config python_files/config_1347.txt --nworkers 5 --rank 2 --ps_ip 172.31.5.74 --ps_port 1347 &> python_files/error_out_1347 &

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
train_set: 10-19 
test_set: 0-9" > python_files/config_1351.txt

nohup ./parameter_server --config python_files/config_1351.txt --nworkers 10 --rank 1 --ps_port 1351 &> python_files/ps_out_1351 & 

nohup ./parameter_server --config python_files/config_1351.txt --nworkers 5 --rank 2 --ps_ip 172.31.5.74 --ps_port 1351 &> python_files/error_out_1351 &

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
learning_rate: 0.005000 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: criteo-kaggle-19b 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 10-19 
test_set: 0-9" > python_files/config_1355.txt

nohup ./parameter_server --config python_files/config_1355.txt --nworkers 10 --rank 1 --ps_port 1355 &> python_files/ps_out_1355 & 

nohup ./parameter_server --config python_files/config_1355.txt --nworkers 5 --rank 2 --ps_ip 172.31.5.74 --ps_port 1355 &> python_files/error_out_1355 &

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
learning_rate: 0.004167 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: criteo-kaggle-19b 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 10-19 
test_set: 0-9" > python_files/config_1359.txt

nohup ./parameter_server --config python_files/config_1359.txt --nworkers 10 --rank 1 --ps_port 1359 &> python_files/ps_out_1359 & 

nohup ./parameter_server --config python_files/config_1359.txt --nworkers 5 --rank 2 --ps_ip 172.31.5.74 --ps_port 1359 &> python_files/error_out_1359 &

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
learning_rate: 0.003571 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: criteo-kaggle-19b 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 10-19 
test_set: 0-9" > python_files/config_1363.txt

nohup ./parameter_server --config python_files/config_1363.txt --nworkers 10 --rank 1 --ps_port 1363 &> python_files/ps_out_1363 & 

nohup ./parameter_server --config python_files/config_1363.txt --nworkers 5 --rank 2 --ps_ip 172.31.5.74 --ps_port 1363 &> python_files/error_out_1363 &

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
learning_rate: 0.003125 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: criteo-kaggle-19b 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 10-19 
test_set: 0-9" > python_files/config_1367.txt

nohup ./parameter_server --config python_files/config_1367.txt --nworkers 10 --rank 1 --ps_port 1367 &> python_files/ps_out_1367 & 

nohup ./parameter_server --config python_files/config_1367.txt --nworkers 5 --rank 2 --ps_ip 172.31.5.74 --ps_port 1367 &> python_files/error_out_1367 &

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
learning_rate: 0.002778 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: criteo-kaggle-19b 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 10-19 
test_set: 0-9" > python_files/config_1371.txt

nohup ./parameter_server --config python_files/config_1371.txt --nworkers 10 --rank 1 --ps_port 1371 &> python_files/ps_out_1371 & 

nohup ./parameter_server --config python_files/config_1371.txt --nworkers 5 --rank 2 --ps_ip 172.31.5.74 --ps_port 1371 &> python_files/error_out_1371 &

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
train_set: 0-9 
test_set: 10-19" > python_files/config_1377.txt

nohup ./parameter_server --config python_files/config_1377.txt --nworkers 10 --rank 1 --ps_port 1377 &> python_files/ps_out_1377 & 

nohup ./parameter_server --config python_files/config_1377.txt --nworkers 5 --rank 2 --ps_ip 172.31.5.74 --ps_port 1377 &> python_files/error_out_1377 &

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
train_set: 0-9 
test_set: 10-19" > python_files/config_1381.txt

nohup ./parameter_server --config python_files/config_1381.txt --nworkers 10 --rank 1 --ps_port 1381 &> python_files/ps_out_1381 & 

nohup ./parameter_server --config python_files/config_1381.txt --nworkers 5 --rank 2 --ps_ip 172.31.5.74 --ps_port 1381 &> python_files/error_out_1381 &

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
train_set: 0-9 
test_set: 10-19" > python_files/config_1385.txt

nohup ./parameter_server --config python_files/config_1385.txt --nworkers 10 --rank 1 --ps_port 1385 &> python_files/ps_out_1385 & 

nohup ./parameter_server --config python_files/config_1385.txt --nworkers 5 --rank 2 --ps_ip 172.31.5.74 --ps_port 1385 &> python_files/error_out_1385 &

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
train_set: 0-9 
test_set: 10-19" > python_files/config_1389.txt

nohup ./parameter_server --config python_files/config_1389.txt --nworkers 10 --rank 1 --ps_port 1389 &> python_files/ps_out_1389 & 

nohup ./parameter_server --config python_files/config_1389.txt --nworkers 5 --rank 2 --ps_ip 172.31.5.74 --ps_port 1389 &> python_files/error_out_1389 &

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
learning_rate: 0.005000 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: criteo-kaggle-19b 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-9 
test_set: 10-19" > python_files/config_1393.txt

nohup ./parameter_server --config python_files/config_1393.txt --nworkers 10 --rank 1 --ps_port 1393 &> python_files/ps_out_1393 & 

nohup ./parameter_server --config python_files/config_1393.txt --nworkers 5 --rank 2 --ps_ip 172.31.5.74 --ps_port 1393 &> python_files/error_out_1393 &

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
learning_rate: 0.004167 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: criteo-kaggle-19b 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-9 
test_set: 10-19" > python_files/config_1397.txt

nohup ./parameter_server --config python_files/config_1397.txt --nworkers 10 --rank 1 --ps_port 1397 &> python_files/ps_out_1397 & 

nohup ./parameter_server --config python_files/config_1397.txt --nworkers 5 --rank 2 --ps_ip 172.31.5.74 --ps_port 1397 &> python_files/error_out_1397 &

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
learning_rate: 0.003571 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: criteo-kaggle-19b 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-9 
test_set: 10-19" > python_files/config_1401.txt

nohup ./parameter_server --config python_files/config_1401.txt --nworkers 10 --rank 1 --ps_port 1401 &> python_files/ps_out_1401 & 

nohup ./parameter_server --config python_files/config_1401.txt --nworkers 5 --rank 2 --ps_ip 172.31.5.74 --ps_port 1401 &> python_files/error_out_1401 &

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
learning_rate: 0.003125 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: criteo-kaggle-19b 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-9 
test_set: 10-19" > python_files/config_1405.txt

nohup ./parameter_server --config python_files/config_1405.txt --nworkers 10 --rank 1 --ps_port 1405 &> python_files/ps_out_1405 & 

nohup ./parameter_server --config python_files/config_1405.txt --nworkers 5 --rank 2 --ps_ip 172.31.5.74 --ps_port 1405 &> python_files/error_out_1405 &

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
learning_rate: 0.002778 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: criteo-kaggle-19b 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-9 
test_set: 10-19" > python_files/config_1409.txt

nohup ./parameter_server --config python_files/config_1409.txt --nworkers 10 --rank 1 --ps_port 1409 &> python_files/ps_out_1409 & 

nohup ./parameter_server --config python_files/config_1409.txt --nworkers 5 --rank 2 --ps_ip 172.31.5.74 --ps_port 1409 &> python_files/error_out_1409 &

