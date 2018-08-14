import cirrus

class CrossValidationTask():
	def __init__(self,
            n_workers,
            lambda_size,
            n_ps,
            worker_size,
            dataset,
            learning_rate,
            epsilon,
            key_name, key_path, # aws key
            ps_ip_public, # public parameter server ip
            ps_ip_private, # private parameter server ip
            ps_ip_port,
            ps_username, # parameter server VM username
            opt_method, # adagrad, sgd, nesterov, momentum
            checkpoint_model, # checkpoint model every x seconds
            num_sets,
            minibatch_size,
            model_bits,
            use_grad_threshold,
            grad_threshold,
            timeout,
            threshold_loss,
            progress_callback
            ):
        self.n_workers = n_workers
        self.lambda_size = lambda_size
        self.n_ps = n_ps
        self.worker_size = worker_size
        self.dataset=dataset
        self.learning_rate = learning_rate
        self.epsilon = epsilon
        self.key_name = key_name
        self.key_path = key_path
        self.ps_ip_public = ps_ip_public
        self.ps_ip_private = ps_ip_private
        self.ps_ip_port = ps_ip_port
        self.ps_username = ps_username
        self.opt_method = opt_method
        self.checkpoint_model = checkpoint_model
        self.num_sets = num_sets
        self.minibatch_size=minibatch_size
        self.model_bits=model_bits
        self.use_grad_threshold=use_grad_threshold
        self.grad_threshold=grad_threshold
        self.timeout=timeout
        self.threshold_loss=threshold_loss
        self.progress_callback=progress_callback
        self.models = []
        self.running = False

    def run(self):
    	if self.running:
    		print "Already running"
    		return
    	set_size = self.num_sets // 10
    	pairs = [(i * set_size, (i + 1) * set_size - 1) for i in range(10)]
    	for i in range(10):
              curr = cirrus.LogisticRegression(
                                self.n_workers,
                                self.n_ps,
                                self.worker_size,
                                self.dataset,
                                self.learning_rate, self.epsilon,
                                self.progress_callback,
                                'model_v1',
                                self.key_name,
                                self.key_path,
                                pairs[:i] + pairs[i + 1:],
                                pairs[i],
                                self.minibatch_size,
                                self.model_bits,
                                self.ps_ip_public,
                                self.ps_ip_private,
                                self.ps_ip_port,
                                self.ps_username,
                                self.opt_method,
                                self.checkpoint_model,
                                self.use_grad_threshold,
                                self.grad_threshold,
                                self.timeout,
                                self.threshold_loss
                                )
    		curr.run()
        	self.models.append(curr)
        	self.running = True

    def get_loss():
    	losses = [self.models[i].get_time_loss(True) for i in range(len(self.models))]
    	return sum(losses) / len(self.models)
