from context import cirrus

urls = [
        "ec2-18-237-213-139.us-west-2.compute.amazonaws.com", 
        "ec2-18-237-31-107.us-west-2.compute.amazonaws.com"]
ips = [
       "172.31.14.190",
       "172.31.6.212"]
data_bucket = 'criteo-kaggle-19b'
model = 'model_v1'

basic_params = {
    'n_workers': 20,
    'n_ps': 1,
    'worker_size': 128,
    'dataset': data_bucket,
    'learning_rate': 0.01,
    'epsilon': 0.0001,
    'progress_callback': None,
    'timeout': 0,
    'threshold_loss': 0,
    'resume_model': model,
    'key_name': 'mykey',
    'key_path': '/home/camus/Downloads/mykey.pem',
    'ps_username': 'ubuntu',
    'opt_method': 'adagrad',
    'checkpoint_model': 60,
    'minibatch_size': 20,
    'model_bits': 19,
    'use_grad_threshold': False,
    'grad_threshold': 0.001,
    'train_set': [(0,824)],
    'test_set': (835,840)
}


if __name__ == "__main__":
    batch = []
    index = 0
    base_port = 1337
    start =    0.100000
    end =      0.000001
    interval = 0.001

        
    machines = zip(urls, ips)

    learning_rates = [0.5/(i * 10) for i in range(1, 20)]

    gs = cirrus.CrossValidation(task=cirrus.GridSearch,
                           param_base=basic_params,
                           hyper_vars=["learning_rate", "worker_size"],
                           hyper_params=[learning_rates, [128, 246, 512]],
                           machines=machines,
                           num_sets=20)
    gs.run(UI=True)


    while True:
        pass
