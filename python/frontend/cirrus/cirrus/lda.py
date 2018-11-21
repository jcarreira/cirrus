# Latent Dirichlet Allocation

from core import BaseTask

class LDATask(BaseTask):
    def __init__(self,
                 k, s3_size, slice_size,
                 *args, **kwargs):
        # pass all arguments of init to parent class
        super(LogisticRegressionTask, self).__init__(*args, **kwargs)

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

         print (config)

         return config

def LDA(

)
