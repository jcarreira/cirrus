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
test_set: 756-840" > config_2363.txt

nohup ./parameter_server --config config_2363.txt --nworkers 40 --rank 1 --ps_port 2363 &> ps_out_2363 & 

nohup ./parameter_server --config config_2363.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 2363 &> error_out_2363 &

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
test_set: 756-840" > config_2367.txt

nohup ./parameter_server --config config_2367.txt --nworkers 40 --rank 1 --ps_port 2367 &> ps_out_2367 & 

nohup ./parameter_server --config config_2367.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 2367 &> error_out_2367 &

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
test_set: 756-840" > config_2371.txt

nohup ./parameter_server --config config_2371.txt --nworkers 40 --rank 1 --ps_port 2371 &> ps_out_2371 & 

nohup ./parameter_server --config config_2371.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 2371 &> error_out_2371 &

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
test_set: 756-840" > config_2375.txt

nohup ./parameter_server --config config_2375.txt --nworkers 40 --rank 1 --ps_port 2375 &> ps_out_2375 & 

nohup ./parameter_server --config config_2375.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 2375 &> error_out_2375 &

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
test_set: 756-840" > config_2379.txt

nohup ./parameter_server --config config_2379.txt --nworkers 40 --rank 1 --ps_port 2379 &> ps_out_2379 & 

nohup ./parameter_server --config config_2379.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 2379 &> error_out_2379 &

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
test_set: 756-840" > config_2383.txt

nohup ./parameter_server --config config_2383.txt --nworkers 40 --rank 1 --ps_port 2383 &> ps_out_2383 & 

nohup ./parameter_server --config config_2383.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 2383 &> error_out_2383 &

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
test_set: 756-840" > config_2387.txt

nohup ./parameter_server --config config_2387.txt --nworkers 40 --rank 1 --ps_port 2387 &> ps_out_2387 & 

nohup ./parameter_server --config config_2387.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 2387 &> error_out_2387 &

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
test_set: 756-840" > config_2391.txt

nohup ./parameter_server --config config_2391.txt --nworkers 40 --rank 1 --ps_port 2391 &> ps_out_2391 & 

nohup ./parameter_server --config config_2391.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 2391 &> error_out_2391 &

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
test_set: 756-840" > config_2395.txt

nohup ./parameter_server --config config_2395.txt --nworkers 40 --rank 1 --ps_port 2395 &> ps_out_2395 & 

nohup ./parameter_server --config config_2395.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 2395 &> error_out_2395 &

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
test_set: 756-840" > config_2399.txt

nohup ./parameter_server --config config_2399.txt --nworkers 40 --rank 1 --ps_port 2399 &> ps_out_2399 & 

nohup ./parameter_server --config config_2399.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 2399 &> error_out_2399 &

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
test_set: 756-840" > config_2403.txt

nohup ./parameter_server --config config_2403.txt --nworkers 40 --rank 1 --ps_port 2403 &> ps_out_2403 & 

nohup ./parameter_server --config config_2403.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 2403 &> error_out_2403 &

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
test_set: 756-840" > config_2407.txt

nohup ./parameter_server --config config_2407.txt --nworkers 40 --rank 1 --ps_port 2407 &> ps_out_2407 & 

nohup ./parameter_server --config config_2407.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 2407 &> error_out_2407 &

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
test_set: 756-840" > config_2411.txt

nohup ./parameter_server --config config_2411.txt --nworkers 40 --rank 1 --ps_port 2411 &> ps_out_2411 & 

nohup ./parameter_server --config config_2411.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 2411 &> error_out_2411 &

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
test_set: 756-840" > config_2415.txt

nohup ./parameter_server --config config_2415.txt --nworkers 40 --rank 1 --ps_port 2415 &> ps_out_2415 & 

nohup ./parameter_server --config config_2415.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 2415 &> error_out_2415 &

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
test_set: 756-840" > config_2419.txt

nohup ./parameter_server --config config_2419.txt --nworkers 40 --rank 1 --ps_port 2419 &> ps_out_2419 & 

nohup ./parameter_server --config config_2419.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 2419 &> error_out_2419 &

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
test_set: 756-840" > config_2423.txt

nohup ./parameter_server --config config_2423.txt --nworkers 40 --rank 1 --ps_port 2423 &> ps_out_2423 & 

nohup ./parameter_server --config config_2423.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 2423 &> error_out_2423 &

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
test_set: 756-840" > config_2427.txt

nohup ./parameter_server --config config_2427.txt --nworkers 40 --rank 1 --ps_port 2427 &> ps_out_2427 & 

nohup ./parameter_server --config config_2427.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 2427 &> error_out_2427 &

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
test_set: 756-840" > config_2431.txt

nohup ./parameter_server --config config_2431.txt --nworkers 40 --rank 1 --ps_port 2431 &> ps_out_2431 & 

nohup ./parameter_server --config config_2431.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 2431 &> error_out_2431 &

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
test_set: 756-840" > config_2435.txt

nohup ./parameter_server --config config_2435.txt --nworkers 40 --rank 1 --ps_port 2435 &> ps_out_2435 & 

nohup ./parameter_server --config config_2435.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 2435 &> error_out_2435 &

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
test_set: 756-840" > config_2439.txt

nohup ./parameter_server --config config_2439.txt --nworkers 40 --rank 1 --ps_port 2439 &> ps_out_2439 & 

nohup ./parameter_server --config config_2439.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 2439 &> error_out_2439 &

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
test_set: 756-840" > config_2443.txt

nohup ./parameter_server --config config_2443.txt --nworkers 40 --rank 1 --ps_port 2443 &> ps_out_2443 & 

nohup ./parameter_server --config config_2443.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 2443 &> error_out_2443 &

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
test_set: 756-840" > config_2447.txt

nohup ./parameter_server --config config_2447.txt --nworkers 40 --rank 1 --ps_port 2447 &> ps_out_2447 & 

nohup ./parameter_server --config config_2447.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 2447 &> error_out_2447 &

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
test_set: 756-840" > config_2451.txt

nohup ./parameter_server --config config_2451.txt --nworkers 40 --rank 1 --ps_port 2451 &> ps_out_2451 & 

nohup ./parameter_server --config config_2451.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 2451 &> error_out_2451 &

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
test_set: 756-840" > config_2455.txt

nohup ./parameter_server --config config_2455.txt --nworkers 40 --rank 1 --ps_port 2455 &> ps_out_2455 & 

nohup ./parameter_server --config config_2455.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 2455 &> error_out_2455 &

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
test_set: 756-840" > config_2459.txt

nohup ./parameter_server --config config_2459.txt --nworkers 40 --rank 1 --ps_port 2459 &> ps_out_2459 & 

nohup ./parameter_server --config config_2459.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 2459 &> error_out_2459 &

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
test_set: 756-840" > config_2463.txt

nohup ./parameter_server --config config_2463.txt --nworkers 40 --rank 1 --ps_port 2463 &> ps_out_2463 & 

nohup ./parameter_server --config config_2463.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 2463 &> error_out_2463 &

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
test_set: 756-840" > config_2467.txt

nohup ./parameter_server --config config_2467.txt --nworkers 40 --rank 1 --ps_port 2467 &> ps_out_2467 & 

nohup ./parameter_server --config config_2467.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 2467 &> error_out_2467 &

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
test_set: 756-840" > config_2471.txt

nohup ./parameter_server --config config_2471.txt --nworkers 40 --rank 1 --ps_port 2471 &> ps_out_2471 & 

nohup ./parameter_server --config config_2471.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 2471 &> error_out_2471 &

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
test_set: 756-840" > config_2475.txt

nohup ./parameter_server --config config_2475.txt --nworkers 40 --rank 1 --ps_port 2475 &> ps_out_2475 & 

nohup ./parameter_server --config config_2475.txt --nworkers 20 --rank 2 --ps_ip 172.31.14.190 --ps_port 2475 &> error_out_2475 &

