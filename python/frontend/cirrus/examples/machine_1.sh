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
train_set: 0-83,84-167,168-251,252-335,336-419,420-503,504-587,588-671,672-755 
test_set: 756-840" > config_2365.txt

nohup ./parameter_server --config config_2365.txt --nworkers 40 --rank 1 --ps_port 2365 &> ps_out_2365 & 

nohup ./parameter_server --config config_2365.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 2365 &> error_out_2365 &

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
train_set: 0-83,84-167,168-251,252-335,336-419,420-503,504-587,588-671,672-755 
test_set: 756-840" > config_2369.txt

nohup ./parameter_server --config config_2369.txt --nworkers 40 --rank 1 --ps_port 2369 &> ps_out_2369 & 

nohup ./parameter_server --config config_2369.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 2369 &> error_out_2369 &

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
train_set: 0-83,84-167,168-251,252-335,336-419,420-503,504-587,588-671,672-755 
test_set: 756-840" > config_2373.txt

nohup ./parameter_server --config config_2373.txt --nworkers 40 --rank 1 --ps_port 2373 &> ps_out_2373 & 

nohup ./parameter_server --config config_2373.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 2373 &> error_out_2373 &

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
train_set: 0-83,84-167,168-251,252-335,336-419,420-503,504-587,588-671,672-755 
test_set: 756-840" > config_2377.txt

nohup ./parameter_server --config config_2377.txt --nworkers 40 --rank 1 --ps_port 2377 &> ps_out_2377 & 

nohup ./parameter_server --config config_2377.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 2377 &> error_out_2377 &

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
train_set: 0-83,84-167,168-251,252-335,336-419,420-503,504-587,588-671,672-755 
test_set: 756-840" > config_2381.txt

nohup ./parameter_server --config config_2381.txt --nworkers 40 --rank 1 --ps_port 2381 &> ps_out_2381 & 

nohup ./parameter_server --config config_2381.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 2381 &> error_out_2381 &

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
train_set: 0-83,84-167,168-251,252-335,336-419,420-503,504-587,588-671,672-755 
test_set: 756-840" > config_2385.txt

nohup ./parameter_server --config config_2385.txt --nworkers 40 --rank 1 --ps_port 2385 &> ps_out_2385 & 

nohup ./parameter_server --config config_2385.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 2385 &> error_out_2385 &

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
train_set: 0-83,84-167,168-251,252-335,336-419,420-503,504-587,588-671,672-755 
test_set: 756-840" > config_2389.txt

nohup ./parameter_server --config config_2389.txt --nworkers 40 --rank 1 --ps_port 2389 &> ps_out_2389 & 

nohup ./parameter_server --config config_2389.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 2389 &> error_out_2389 &

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
train_set: 0-83,84-167,168-251,252-335,336-419,420-503,504-587,588-671,672-755 
test_set: 756-840" > config_2393.txt

nohup ./parameter_server --config config_2393.txt --nworkers 40 --rank 1 --ps_port 2393 &> ps_out_2393 & 

nohup ./parameter_server --config config_2393.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 2393 &> error_out_2393 &

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
train_set: 0-83,84-167,168-251,252-335,336-419,420-503,504-587,588-671,672-755 
test_set: 756-840" > config_2397.txt

nohup ./parameter_server --config config_2397.txt --nworkers 40 --rank 1 --ps_port 2397 &> ps_out_2397 & 

nohup ./parameter_server --config config_2397.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 2397 &> error_out_2397 &

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
train_set: 0-83,84-167,168-251,252-335,336-419,420-503,504-587,588-671,672-755 
test_set: 756-840" > config_2401.txt

nohup ./parameter_server --config config_2401.txt --nworkers 40 --rank 1 --ps_port 2401 &> ps_out_2401 & 

nohup ./parameter_server --config config_2401.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 2401 &> error_out_2401 &

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
train_set: 0-83,84-167,168-251,252-335,336-419,420-503,504-587,588-671,672-755 
test_set: 756-840" > config_2405.txt

nohup ./parameter_server --config config_2405.txt --nworkers 40 --rank 1 --ps_port 2405 &> ps_out_2405 & 

nohup ./parameter_server --config config_2405.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 2405 &> error_out_2405 &

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
train_set: 0-83,84-167,168-251,252-335,336-419,420-503,504-587,588-671,672-755 
test_set: 756-840" > config_2409.txt

nohup ./parameter_server --config config_2409.txt --nworkers 40 --rank 1 --ps_port 2409 &> ps_out_2409 & 

nohup ./parameter_server --config config_2409.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 2409 &> error_out_2409 &

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
train_set: 0-83,84-167,168-251,252-335,336-419,420-503,504-587,588-671,672-755 
test_set: 756-840" > config_2413.txt

nohup ./parameter_server --config config_2413.txt --nworkers 40 --rank 1 --ps_port 2413 &> ps_out_2413 & 

nohup ./parameter_server --config config_2413.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 2413 &> error_out_2413 &

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
train_set: 0-83,84-167,168-251,252-335,336-419,420-503,504-587,588-671,672-755 
test_set: 756-840" > config_2417.txt

nohup ./parameter_server --config config_2417.txt --nworkers 40 --rank 1 --ps_port 2417 &> ps_out_2417 & 

nohup ./parameter_server --config config_2417.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 2417 &> error_out_2417 &

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
train_set: 0-83,84-167,168-251,252-335,336-419,420-503,504-587,588-671,672-755 
test_set: 756-840" > config_2421.txt

nohup ./parameter_server --config config_2421.txt --nworkers 40 --rank 1 --ps_port 2421 &> ps_out_2421 & 

nohup ./parameter_server --config config_2421.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 2421 &> error_out_2421 &

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
train_set: 0-83,84-167,168-251,252-335,336-419,420-503,504-587,588-671,672-755 
test_set: 756-840" > config_2425.txt

nohup ./parameter_server --config config_2425.txt --nworkers 40 --rank 1 --ps_port 2425 &> ps_out_2425 & 

nohup ./parameter_server --config config_2425.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 2425 &> error_out_2425 &

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
train_set: 0-83,84-167,168-251,252-335,336-419,420-503,504-587,588-671,672-755 
test_set: 756-840" > config_2429.txt

nohup ./parameter_server --config config_2429.txt --nworkers 40 --rank 1 --ps_port 2429 &> ps_out_2429 & 

nohup ./parameter_server --config config_2429.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 2429 &> error_out_2429 &

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
train_set: 0-83,84-167,168-251,252-335,336-419,420-503,504-587,588-671,672-755 
test_set: 756-840" > config_2433.txt

nohup ./parameter_server --config config_2433.txt --nworkers 40 --rank 1 --ps_port 2433 &> ps_out_2433 & 

nohup ./parameter_server --config config_2433.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 2433 &> error_out_2433 &

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
train_set: 0-83,84-167,168-251,252-335,336-419,420-503,504-587,588-671,672-755 
test_set: 756-840" > config_2437.txt

nohup ./parameter_server --config config_2437.txt --nworkers 40 --rank 1 --ps_port 2437 &> ps_out_2437 & 

nohup ./parameter_server --config config_2437.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 2437 &> error_out_2437 &

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
train_set: 0-83,84-167,168-251,252-335,336-419,420-503,504-587,588-671,672-755 
test_set: 756-840" > config_2441.txt

nohup ./parameter_server --config config_2441.txt --nworkers 40 --rank 1 --ps_port 2441 &> ps_out_2441 & 

nohup ./parameter_server --config config_2441.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 2441 &> error_out_2441 &

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
train_set: 0-83,84-167,168-251,252-335,336-419,420-503,504-587,588-671,672-755 
test_set: 756-840" > config_2445.txt

nohup ./parameter_server --config config_2445.txt --nworkers 40 --rank 1 --ps_port 2445 &> ps_out_2445 & 

nohup ./parameter_server --config config_2445.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 2445 &> error_out_2445 &

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
train_set: 0-83,84-167,168-251,252-335,336-419,420-503,504-587,588-671,672-755 
test_set: 756-840" > config_2449.txt

nohup ./parameter_server --config config_2449.txt --nworkers 40 --rank 1 --ps_port 2449 &> ps_out_2449 & 

nohup ./parameter_server --config config_2449.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 2449 &> error_out_2449 &

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
train_set: 0-83,84-167,168-251,252-335,336-419,420-503,504-587,588-671,672-755 
test_set: 756-840" > config_2453.txt

nohup ./parameter_server --config config_2453.txt --nworkers 40 --rank 1 --ps_port 2453 &> ps_out_2453 & 

nohup ./parameter_server --config config_2453.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 2453 &> error_out_2453 &

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
train_set: 0-83,84-167,168-251,252-335,336-419,420-503,504-587,588-671,672-755 
test_set: 756-840" > config_2457.txt

nohup ./parameter_server --config config_2457.txt --nworkers 40 --rank 1 --ps_port 2457 &> ps_out_2457 & 

nohup ./parameter_server --config config_2457.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 2457 &> error_out_2457 &

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
train_set: 0-83,84-167,168-251,252-335,336-419,420-503,504-587,588-671,672-755 
test_set: 756-840" > config_2461.txt

nohup ./parameter_server --config config_2461.txt --nworkers 40 --rank 1 --ps_port 2461 &> ps_out_2461 & 

nohup ./parameter_server --config config_2461.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 2461 &> error_out_2461 &

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
train_set: 0-83,84-167,168-251,252-335,336-419,420-503,504-587,588-671,672-755 
test_set: 756-840" > config_2465.txt

nohup ./parameter_server --config config_2465.txt --nworkers 40 --rank 1 --ps_port 2465 &> ps_out_2465 & 

nohup ./parameter_server --config config_2465.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 2465 &> error_out_2465 &

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
train_set: 0-83,84-167,168-251,252-335,336-419,420-503,504-587,588-671,672-755 
test_set: 756-840" > config_2469.txt

nohup ./parameter_server --config config_2469.txt --nworkers 40 --rank 1 --ps_port 2469 &> ps_out_2469 & 

nohup ./parameter_server --config config_2469.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 2469 &> error_out_2469 &

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
train_set: 0-83,84-167,168-251,252-335,336-419,420-503,504-587,588-671,672-755 
test_set: 756-840" > config_2473.txt

nohup ./parameter_server --config config_2473.txt --nworkers 40 --rank 1 --ps_port 2473 &> ps_out_2473 & 

nohup ./parameter_server --config config_2473.txt --nworkers 20 --rank 2 --ps_ip 172.31.6.212 --ps_port 2473 &> error_out_2473 &

