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
train_set: 0-9 
test_set: 10-19" > python_files/config_1453.txt

nohup ./parameter_server --config python_files/config_1453.txt --nworkers 40 --rank 1 --ps_port 1453 &> python_files/ps_out_1453 & 

nohup ./parameter_server --config python_files/config_1453.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 1453 &> python_files/error_out_1453 &

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
test_set: 10-19" > python_files/config_1457.txt

nohup ./parameter_server --config python_files/config_1457.txt --nworkers 40 --rank 1 --ps_port 1457 &> python_files/ps_out_1457 & 

nohup ./parameter_server --config python_files/config_1457.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 1457 &> python_files/error_out_1457 &

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
test_set: 10-19" > python_files/config_1461.txt

nohup ./parameter_server --config python_files/config_1461.txt --nworkers 40 --rank 1 --ps_port 1461 &> python_files/ps_out_1461 & 

nohup ./parameter_server --config python_files/config_1461.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 1461 &> python_files/error_out_1461 &

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
train_set: 0-9 
test_set: 10-19" > python_files/config_1465.txt

nohup ./parameter_server --config python_files/config_1465.txt --nworkers 40 --rank 1 --ps_port 1465 &> python_files/ps_out_1465 & 

nohup ./parameter_server --config python_files/config_1465.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 1465 &> python_files/error_out_1465 &

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
test_set: 10-19" > python_files/config_1469.txt

nohup ./parameter_server --config python_files/config_1469.txt --nworkers 40 --rank 1 --ps_port 1469 &> python_files/ps_out_1469 & 

nohup ./parameter_server --config python_files/config_1469.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 1469 &> python_files/error_out_1469 &

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
test_set: 10-19" > python_files/config_1473.txt

nohup ./parameter_server --config python_files/config_1473.txt --nworkers 40 --rank 1 --ps_port 1473 &> python_files/ps_out_1473 & 

nohup ./parameter_server --config python_files/config_1473.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 1473 &> python_files/error_out_1473 &

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
train_set: 0-9 
test_set: 10-19" > python_files/config_1477.txt

nohup ./parameter_server --config python_files/config_1477.txt --nworkers 40 --rank 1 --ps_port 1477 &> python_files/ps_out_1477 & 

nohup ./parameter_server --config python_files/config_1477.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 1477 &> python_files/error_out_1477 &

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
test_set: 10-19" > python_files/config_1481.txt

nohup ./parameter_server --config python_files/config_1481.txt --nworkers 40 --rank 1 --ps_port 1481 &> python_files/ps_out_1481 & 

nohup ./parameter_server --config python_files/config_1481.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 1481 &> python_files/error_out_1481 &

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
test_set: 10-19" > python_files/config_1485.txt

nohup ./parameter_server --config python_files/config_1485.txt --nworkers 40 --rank 1 --ps_port 1485 &> python_files/ps_out_1485 & 

nohup ./parameter_server --config python_files/config_1485.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 1485 &> python_files/error_out_1485 &

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
train_set: 0-9 
test_set: 10-19" > python_files/config_1489.txt

nohup ./parameter_server --config python_files/config_1489.txt --nworkers 40 --rank 1 --ps_port 1489 &> python_files/ps_out_1489 & 

nohup ./parameter_server --config python_files/config_1489.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 1489 &> python_files/error_out_1489 &

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
test_set: 10-19" > python_files/config_1493.txt

nohup ./parameter_server --config python_files/config_1493.txt --nworkers 40 --rank 1 --ps_port 1493 &> python_files/ps_out_1493 & 

nohup ./parameter_server --config python_files/config_1493.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 1493 &> python_files/error_out_1493 &

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
test_set: 10-19" > python_files/config_1497.txt

nohup ./parameter_server --config python_files/config_1497.txt --nworkers 40 --rank 1 --ps_port 1497 &> python_files/ps_out_1497 & 

nohup ./parameter_server --config python_files/config_1497.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 1497 &> python_files/error_out_1497 &

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
train_set: 0-9 
test_set: 10-19" > python_files/config_1501.txt

nohup ./parameter_server --config python_files/config_1501.txt --nworkers 40 --rank 1 --ps_port 1501 &> python_files/ps_out_1501 & 

nohup ./parameter_server --config python_files/config_1501.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 1501 &> python_files/error_out_1501 &

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
test_set: 10-19" > python_files/config_1505.txt

nohup ./parameter_server --config python_files/config_1505.txt --nworkers 40 --rank 1 --ps_port 1505 &> python_files/ps_out_1505 & 

nohup ./parameter_server --config python_files/config_1505.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 1505 &> python_files/error_out_1505 &

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
test_set: 10-19" > python_files/config_1509.txt

nohup ./parameter_server --config python_files/config_1509.txt --nworkers 40 --rank 1 --ps_port 1509 &> python_files/ps_out_1509 & 

nohup ./parameter_server --config python_files/config_1509.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 1509 &> python_files/error_out_1509 &

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
learning_rate: 0.004545 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: criteo-kaggle-19b 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-9 
test_set: 10-19" > python_files/config_1513.txt

nohup ./parameter_server --config python_files/config_1513.txt --nworkers 40 --rank 1 --ps_port 1513 &> python_files/ps_out_1513 & 

nohup ./parameter_server --config python_files/config_1513.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 1513 &> python_files/error_out_1513 &

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
test_set: 10-19" > python_files/config_1517.txt

nohup ./parameter_server --config python_files/config_1517.txt --nworkers 40 --rank 1 --ps_port 1517 &> python_files/ps_out_1517 & 

nohup ./parameter_server --config python_files/config_1517.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 1517 &> python_files/error_out_1517 &

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
test_set: 10-19" > python_files/config_1521.txt

nohup ./parameter_server --config python_files/config_1521.txt --nworkers 40 --rank 1 --ps_port 1521 &> python_files/ps_out_1521 & 

nohup ./parameter_server --config python_files/config_1521.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 1521 &> python_files/error_out_1521 &

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
learning_rate: 0.003846 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: criteo-kaggle-19b 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-9 
test_set: 10-19" > python_files/config_1525.txt

nohup ./parameter_server --config python_files/config_1525.txt --nworkers 40 --rank 1 --ps_port 1525 &> python_files/ps_out_1525 & 

nohup ./parameter_server --config python_files/config_1525.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 1525 &> python_files/error_out_1525 &

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
test_set: 10-19" > python_files/config_1529.txt

nohup ./parameter_server --config python_files/config_1529.txt --nworkers 40 --rank 1 --ps_port 1529 &> python_files/ps_out_1529 & 

nohup ./parameter_server --config python_files/config_1529.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 1529 &> python_files/error_out_1529 &

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
test_set: 10-19" > python_files/config_1533.txt

nohup ./parameter_server --config python_files/config_1533.txt --nworkers 40 --rank 1 --ps_port 1533 &> python_files/ps_out_1533 & 

nohup ./parameter_server --config python_files/config_1533.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 1533 &> python_files/error_out_1533 &

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
learning_rate: 0.003333 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: criteo-kaggle-19b 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-9 
test_set: 10-19" > python_files/config_1537.txt

nohup ./parameter_server --config python_files/config_1537.txt --nworkers 40 --rank 1 --ps_port 1537 &> python_files/ps_out_1537 & 

nohup ./parameter_server --config python_files/config_1537.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 1537 &> python_files/error_out_1537 &

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
test_set: 10-19" > python_files/config_1541.txt

nohup ./parameter_server --config python_files/config_1541.txt --nworkers 40 --rank 1 --ps_port 1541 &> python_files/ps_out_1541 & 

nohup ./parameter_server --config python_files/config_1541.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 1541 &> python_files/error_out_1541 &

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
test_set: 10-19" > python_files/config_1545.txt

nohup ./parameter_server --config python_files/config_1545.txt --nworkers 40 --rank 1 --ps_port 1545 &> python_files/ps_out_1545 & 

nohup ./parameter_server --config python_files/config_1545.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 1545 &> python_files/error_out_1545 &

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
learning_rate: 0.002941 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: criteo-kaggle-19b 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-9 
test_set: 10-19" > python_files/config_1549.txt

nohup ./parameter_server --config python_files/config_1549.txt --nworkers 40 --rank 1 --ps_port 1549 &> python_files/ps_out_1549 & 

nohup ./parameter_server --config python_files/config_1549.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 1549 &> python_files/error_out_1549 &

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
test_set: 10-19" > python_files/config_1553.txt

nohup ./parameter_server --config python_files/config_1553.txt --nworkers 40 --rank 1 --ps_port 1553 &> python_files/ps_out_1553 & 

nohup ./parameter_server --config python_files/config_1553.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 1553 &> python_files/error_out_1553 &

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
test_set: 10-19" > python_files/config_1557.txt

nohup ./parameter_server --config python_files/config_1557.txt --nworkers 40 --rank 1 --ps_port 1557 &> python_files/ps_out_1557 & 

nohup ./parameter_server --config python_files/config_1557.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 1557 &> python_files/error_out_1557 &

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
learning_rate: 0.002632 
epsilon: 0.000100 
model_bits: 19 
dataset_format: binary 
s3_bucket: criteo-kaggle-19b 
use_grad_threshold: 0 
grad_threshold: 0.001000 
train_set: 0-9 
test_set: 10-19" > python_files/config_1561.txt

nohup ./parameter_server --config python_files/config_1561.txt --nworkers 40 --rank 1 --ps_port 1561 &> python_files/ps_out_1561 & 

nohup ./parameter_server --config python_files/config_1561.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 1561 &> python_files/error_out_1561 &

