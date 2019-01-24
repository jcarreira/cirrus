# Latent Dirichlet Allocation

from core import BaseTask
import subprocess
import time
import messenger
import signal
import sys
import os

class LDATask(BaseTask):
    def __init__(self,
                 k, s3_size, slice_size,
                 n_iter,
                 *args, **kwargs):
        # pass all arguments of init to parent class
        super(LDATask, self).__init__(*args, **kwargs)

        def signal_handler(sig, frame):
            os.killpg(os.getpgid(self.cmd.pid), signal.SIGTERM)
            sys.exit(0)

        self.k = k
        self.s3_size = s3_size
        self.slice_size = slice_size

        self.n_iter = n_iter
        self.num_task = 5
        self.lambda_name = "jeff-lambda"
        self.task_id_list = range(self.n_workers)
        self.num_task_list = range(self.num_task, self.num_task + self.n_workers)

        signal.signal(signal.SIGINT, signal_handler)


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

    def launch_ps(self, command_dict=None):
        cmd = './parameter_server --config lda.cfg --nworkers %d --rank 1 --ps_ip %s --ps_port %d &' % (
            self.n_workers, self.ps_ip_private, self.ps_ip_port)
        if command_dict is not None:
            command_dict[self.ps_ip_public].append(cmd)
        else:
            print (cmd)
            self.cmd = subprocess.Popen(cmd, shell=True, preexec_fn=os.setsid)
            # subprocess.call(cmd, shell=True)
            # raise ValueError('SSH Copy config not implemented')

    def get_loglikelihood(self):
        if self.is_dead():
            return

        cur_iter = 0
        if_incre = True

        while True:

            num_lambdas = messenger.get_num_lambdas(self.ps_ip_public, self.ps_ip_port)
            # print ("num_lambda: ", num_lambdas)

            if num_lambdas is not None and num_lambdas == 0:

                if cur_iter == self.n_iter:
                    break

                shortage = self.n_workers - num_lambdas

                print ("running: ", num_lambdas, self.n_workers)
                print ("***task id: ", self.task_id_list[0])

                print ("*** ", shortage)
                for i in range(shortage):
                    payload = '{"num_task": %d, "num_workers": %d, "ps_ip": \"%s\", "ps_port": %d, "task_id": %d}' \
                                % (self.num_task_list[i], self.n_workers, self.ps_ip_private, self.ps_ip_port, self.task_id_list[i])
                    try:
                        response = self.lambda_client.invoke(
                            FunctionName=self.lambda_name,
                            InvocationType='Event',
                            LogType='Tail',
                            Payload=payload)
                    except Exception as e:
                        print "client.invoke exception caught"
                        print str(e)
                self.task_id_list = [i + self.n_workers for i in self.task_id_list]
                time.sleep(2)

                if_incre = False

            else:
                if not if_incre:
                    cur_iter += 1
                    if_incre = True


    def run(self):
        self.define_config()
        self.launch_ps()
        self.get_loglikelihood()
        self.kill()
        # self.relaunch_lambdas()

def LDA(k,
        s3_size,
        slice_size,
        n_iter,
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
                   n_iter = n_iter,
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
