# Softmax

from core import BaseTask


class SoftmaxTask(BaseTask):
    def __init__(self, *args, **kwargs):
        # pass all arguments of init to parent class
        super(SoftmaxTask, self).__init__(*args, **kwargs)

    def __del__(self):
        print("Softmax Task Lost")

    def define_config(self):
        
        if self.use_grad_threshold:
            grad_t = 1
        else:
            grad_t = 0

        config = "load_input_path: /mnt/efs/mnist/train_mnist1.csv \n" + \
                 "load_input_type: csv\n" + \
                 "num_classes: 10 \n" + \
                 "num_features: 784 \n" + \
                 "limit_cols: 1000 \n" + \
                 "normalize: 1 \n" + \
                 "limit_samples: 50000000 \n" + \
                 "s3_size: 1000 \n" + \
                 "use_bias: 1 \n" + \
                 "model_type: Softmax \n" + \
                 "minibatch_size: %d \n" % self.minibatch_size + \
                 "learning_rate: %f \n" % self.learning_rate + \
                 "epsilon: %lf \n" % self.epsilon + \
                 "model_bits: %d \n" % self.model_bits + \
                 "dataset_format: binary \n" + \
                 "s3_bucket: %s \n" % self.dataset + \
                 "use_grad_threshold: %d \n" % grad_t + \
                 "grad_threshold: %lf \n" % self.grad_threshold + \
                 "train_set: %d-%d \n" % self.train_set + \
                 "test_set: %d-%d" % self.test_set
        return config


def Softmax(
            n_workers,
            n_ps,
            worker_size,
            dataset,
            learning_rate, epsilon,
            progress_callback,
            resume_model,
            key_name,
            key_path,
            train_set,
            test_set,
            minibatch_size,
            model_bits,
            ps_ip_public="",
            ps_ip_private="",
            ps_ip_port=1337,
            ps_username="ec2-user",
            opt_method="sgd",
            checkpoint_model=0,
            use_grad_threshold=False,
            grad_threshold=0.001,
            timeout=60,
            threshold_loss=0
            ):
    return SoftmaxTask(
            n_workers=n_workers,
            lambda_size=10,
            n_ps=n_ps,
            worker_size=worker_size,
            dataset=dataset,
            learning_rate=learning_rate,
            epsilon=epsilon,
            key_name=key_name,
            key_path=key_path,
            ps_ip_public=ps_ip_public,
            ps_ip_private=ps_ip_private,
            ps_ip_port=ps_ip_port,
            ps_username=ps_username,
            opt_method=opt_method,
            checkpoint_model=checkpoint_model,
            train_set=train_set,
            test_set=test_set,
            minibatch_size=minibatch_size,
            model_bits=model_bits,
            use_grad_threshold=use_grad_threshold,
            grad_threshold=grad_threshold,
            timeout=timeout,
            threshold_loss=threshold_loss,
            progress_callback=progress_callback
           )
