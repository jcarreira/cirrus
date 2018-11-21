# Latent Dirichlet Allocation

from core import BaseTask

class LDATask(BaseTask):
    def __init__(self,
                 k, s3_size, slice_size,
                 *args, **kwargs):
        # pass all arguments of init to parent class
        super(LDATask, self).__init__(*args, **kwargs)

        self.k = k
        self.s3_size = s3_size
        self.slice_size = slice_size

        self.num_task = 5


    def __del__(self):
        print("LDA Task Lost")

    def define_config(self):

        config = "dataset_format: lda \n" + \
                 "vocab_path: nytimes_vocab.txt \n" + \
                 "doc_path: nytimes.txt\n" + \
                 "K: %d \n" % self.k + \
                 "limit_samples: 50000000 \n" + \
                 "s3_size: %d \n" % self.s3_size + \
                 "model_type: LDA \n" + \
                 "minibatch_size: 100 \n" + \
                 "s3_bucket: %s \n" % self.dataset + \
                 "train_set: %d-%d \n" % self.train_set + \
                 "slice_size: %d" % self.slice_size

        print config

        return config

def LDA(k,
        s3_size,
        slice_size,
        n_workers,
        lambda_size,
        n_ps,
        dataset,
        progress_callback,
        key_name,
        key_path,
        ps_ip_public,
        ps_ip_private,
        ps_ip_port,
        ps_username,
        minibatch_size,
        train_set):
        return LDATask(k = k,
                   s3_size = s3_size,
                   slice_size = slice_size,
                   n_workers = n_workers,
                   lambda_size = lambda_size,
                   n_ps = n_ps,
                   dataset = dataset,
                   progress_callback = progress_callback,
                   key_name = key_name,
                   key_path = key_path,
                   ps_ip_public = ps_ip_public,
                   ps_ip_private = ps_ip_private,
                   ps_ip_port = ps_ip_port,
                   ps_username = ps_username,
                   minibatch_size = minibatch_size,
                   train_set = train_set)
