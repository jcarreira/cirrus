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
test_set: 10-19" > python_files/config_1451.txt

nohup ./parameter_server --config python_files/config_1451.txt --nworkers 40 --rank 1 --ps_port 1451 &> python_files/ps_out_1451 & 

nohup ./parameter_server --config python_files/config_1451.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 1451 &> python_files/error_out_1451 &

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
test_set: 10-19" > python_files/config_1455.txt

nohup ./parameter_server --config python_files/config_1455.txt --nworkers 40 --rank 1 --ps_port 1455 &> python_files/ps_out_1455 & 

nohup ./parameter_server --config python_files/config_1455.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 1455 &> python_files/error_out_1455 &

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
test_set: 10-19" > python_files/config_1459.txt

nohup ./parameter_server --config python_files/config_1459.txt --nworkers 40 --rank 1 --ps_port 1459 &> python_files/ps_out_1459 & 

nohup ./parameter_server --config python_files/config_1459.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 1459 &> python_files/error_out_1459 &

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
test_set: 10-19" > python_files/config_1463.txt

nohup ./parameter_server --config python_files/config_1463.txt --nworkers 40 --rank 1 --ps_port 1463 &> python_files/ps_out_1463 & 

nohup ./parameter_server --config python_files/config_1463.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 1463 &> python_files/error_out_1463 &

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
test_set: 10-19" > python_files/config_1467.txt

nohup ./parameter_server --config python_files/config_1467.txt --nworkers 40 --rank 1 --ps_port 1467 &> python_files/ps_out_1467 & 

nohup ./parameter_server --config python_files/config_1467.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 1467 &> python_files/error_out_1467 &

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
test_set: 10-19" > python_files/config_1471.txt

nohup ./parameter_server --config python_files/config_1471.txt --nworkers 40 --rank 1 --ps_port 1471 &> python_files/ps_out_1471 & 

nohup ./parameter_server --config python_files/config_1471.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 1471 &> python_files/error_out_1471 &

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
test_set: 10-19" > python_files/config_1475.txt

nohup ./parameter_server --config python_files/config_1475.txt --nworkers 40 --rank 1 --ps_port 1475 &> python_files/ps_out_1475 & 

nohup ./parameter_server --config python_files/config_1475.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 1475 &> python_files/error_out_1475 &

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
test_set: 10-19" > python_files/config_1479.txt

nohup ./parameter_server --config python_files/config_1479.txt --nworkers 40 --rank 1 --ps_port 1479 &> python_files/ps_out_1479 & 

nohup ./parameter_server --config python_files/config_1479.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 1479 &> python_files/error_out_1479 &

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
test_set: 10-19" > python_files/config_1483.txt

nohup ./parameter_server --config python_files/config_1483.txt --nworkers 40 --rank 1 --ps_port 1483 &> python_files/ps_out_1483 & 

nohup ./parameter_server --config python_files/config_1483.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 1483 &> python_files/error_out_1483 &

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
test_set: 10-19" > python_files/config_1487.txt

nohup ./parameter_server --config python_files/config_1487.txt --nworkers 40 --rank 1 --ps_port 1487 &> python_files/ps_out_1487 & 

nohup ./parameter_server --config python_files/config_1487.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 1487 &> python_files/error_out_1487 &

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
test_set: 10-19" > python_files/config_1491.txt

nohup ./parameter_server --config python_files/config_1491.txt --nworkers 40 --rank 1 --ps_port 1491 &> python_files/ps_out_1491 & 

nohup ./parameter_server --config python_files/config_1491.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 1491 &> python_files/error_out_1491 &

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
test_set: 10-19" > python_files/config_1495.txt

nohup ./parameter_server --config python_files/config_1495.txt --nworkers 40 --rank 1 --ps_port 1495 &> python_files/ps_out_1495 & 

nohup ./parameter_server --config python_files/config_1495.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 1495 &> python_files/error_out_1495 &

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
test_set: 10-19" > python_files/config_1499.txt

nohup ./parameter_server --config python_files/config_1499.txt --nworkers 40 --rank 1 --ps_port 1499 &> python_files/ps_out_1499 & 

nohup ./parameter_server --config python_files/config_1499.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 1499 &> python_files/error_out_1499 &

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
test_set: 10-19" > python_files/config_1503.txt

nohup ./parameter_server --config python_files/config_1503.txt --nworkers 40 --rank 1 --ps_port 1503 &> python_files/ps_out_1503 & 

nohup ./parameter_server --config python_files/config_1503.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 1503 &> python_files/error_out_1503 &

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
test_set: 10-19" > python_files/config_1507.txt

nohup ./parameter_server --config python_files/config_1507.txt --nworkers 40 --rank 1 --ps_port 1507 &> python_files/ps_out_1507 & 

nohup ./parameter_server --config python_files/config_1507.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 1507 &> python_files/error_out_1507 &

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
test_set: 10-19" > python_files/config_1511.txt

nohup ./parameter_server --config python_files/config_1511.txt --nworkers 40 --rank 1 --ps_port 1511 &> python_files/ps_out_1511 & 

nohup ./parameter_server --config python_files/config_1511.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 1511 &> python_files/error_out_1511 &

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
test_set: 10-19" > python_files/config_1515.txt

nohup ./parameter_server --config python_files/config_1515.txt --nworkers 40 --rank 1 --ps_port 1515 &> python_files/ps_out_1515 & 

nohup ./parameter_server --config python_files/config_1515.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 1515 &> python_files/error_out_1515 &

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
test_set: 10-19" > python_files/config_1519.txt

nohup ./parameter_server --config python_files/config_1519.txt --nworkers 40 --rank 1 --ps_port 1519 &> python_files/ps_out_1519 & 

nohup ./parameter_server --config python_files/config_1519.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 1519 &> python_files/error_out_1519 &

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
test_set: 10-19" > python_files/config_1523.txt

nohup ./parameter_server --config python_files/config_1523.txt --nworkers 40 --rank 1 --ps_port 1523 &> python_files/ps_out_1523 & 

nohup ./parameter_server --config python_files/config_1523.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 1523 &> python_files/error_out_1523 &

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
test_set: 10-19" > python_files/config_1527.txt

nohup ./parameter_server --config python_files/config_1527.txt --nworkers 40 --rank 1 --ps_port 1527 &> python_files/ps_out_1527 & 

nohup ./parameter_server --config python_files/config_1527.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 1527 &> python_files/error_out_1527 &

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
test_set: 10-19" > python_files/config_1531.txt

nohup ./parameter_server --config python_files/config_1531.txt --nworkers 40 --rank 1 --ps_port 1531 &> python_files/ps_out_1531 & 

nohup ./parameter_server --config python_files/config_1531.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 1531 &> python_files/error_out_1531 &

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
test_set: 10-19" > python_files/config_1535.txt

nohup ./parameter_server --config python_files/config_1535.txt --nworkers 40 --rank 1 --ps_port 1535 &> python_files/ps_out_1535 & 

nohup ./parameter_server --config python_files/config_1535.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 1535 &> python_files/error_out_1535 &

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
test_set: 10-19" > python_files/config_1539.txt

nohup ./parameter_server --config python_files/config_1539.txt --nworkers 40 --rank 1 --ps_port 1539 &> python_files/ps_out_1539 & 

nohup ./parameter_server --config python_files/config_1539.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 1539 &> python_files/error_out_1539 &

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
test_set: 10-19" > python_files/config_1543.txt

nohup ./parameter_server --config python_files/config_1543.txt --nworkers 40 --rank 1 --ps_port 1543 &> python_files/ps_out_1543 & 

nohup ./parameter_server --config python_files/config_1543.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 1543 &> python_files/error_out_1543 &

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
test_set: 10-19" > python_files/config_1547.txt

nohup ./parameter_server --config python_files/config_1547.txt --nworkers 40 --rank 1 --ps_port 1547 &> python_files/ps_out_1547 & 

nohup ./parameter_server --config python_files/config_1547.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 1547 &> python_files/error_out_1547 &

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
test_set: 10-19" > python_files/config_1551.txt

nohup ./parameter_server --config python_files/config_1551.txt --nworkers 40 --rank 1 --ps_port 1551 &> python_files/ps_out_1551 & 

nohup ./parameter_server --config python_files/config_1551.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 1551 &> python_files/error_out_1551 &

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
test_set: 10-19" > python_files/config_1555.txt

nohup ./parameter_server --config python_files/config_1555.txt --nworkers 40 --rank 1 --ps_port 1555 &> python_files/ps_out_1555 & 

nohup ./parameter_server --config python_files/config_1555.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 1555 &> python_files/error_out_1555 &

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
test_set: 10-19" > python_files/config_1559.txt

nohup ./parameter_server --config python_files/config_1559.txt --nworkers 40 --rank 1 --ps_port 1559 &> python_files/ps_out_1559 & 

nohup ./parameter_server --config python_files/config_1559.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 1559 &> python_files/error_out_1559 &

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
test_set: 10-19" > python_files/config_1563.txt

nohup ./parameter_server --config python_files/config_1563.txt --nworkers 40 --rank 1 --ps_port 1563 &> python_files/ps_out_1563 & 

nohup ./parameter_server --config python_files/config_1563.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 1563 &> python_files/error_out_1563 &

