# Logistic Regression

from core import BaseTask
import ConfigGenerator


class LogisticRegressionTask(BaseTask):
    def __init__(self, *args, **kwargs):
        super(LogisticRegressionTask, self).__init__(*args, **kwargs)

    def __del__(self):
        print("Logistic Regression Task Lost")

    def define_config(self):
        
        if self.use_grad_threshold:
            grad_t = 1
        else:
            grad_t = 0

        config = ConfigGenerator.config_gen(
                s3_size = 50000,
                model_type = "LogisticRegression",
                minibatch_size = self.minibatch_size,
                learning_rate = self.learning_rate,
                epsilon = self.epsilon,
                model_bits = self.model_bits,
                s3_bucket = self.dataset,
                use_grad_threshold = grad_t,
                grad_threshold = self.grad_threshold,
                train_set = "%d-%d" % self.train_set,
                test_set = "%d-%d" % self.test_set)
        return config


def LogisticRegression(
            n_workers,
            n_ps,
            lambda_size,
            dataset,
            learning_rate, 
            progress_callback,
            key_name,
            key_path,
            train_set,
            test_set,
            model_bits,
            epsilon=0,
            resume_model="",
            minibatch_size=20,
            ps_ip_public="",
            ps_ip_private="",
            ps_ip_port=1337,
            ps_username="ubuntu",
            opt_method="sgd",
            checkpoint_model=0,
            use_grad_threshold=False,
            grad_threshold=0.001,
            timeout=60,
            threshold_loss=0
            ):
    return LogisticRegressionTask(
            n_workers=n_workers,
            n_ps=1,
            lambda_size=lambda_size,
            n_ps=n_ps,
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

