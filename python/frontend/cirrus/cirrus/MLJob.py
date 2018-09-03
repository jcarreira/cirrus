from abc import ABCMeta, abstractmethod

class MLJob(object):
  def __init__(self, task = None):
    __metaclass__ = ABCMeta
      
    self.loss_lst = []
    self.cirrus_objs = []
    self.task = task
    
  @abstractmethod
  def get_xs_for(self, i, metric = "LOSS"):
    pass

  @abstractmethod
  def get_ys_for(self, i, metric = "LOSS"):
    pass

  @abstractmethod
  def run(self, UI=False):
    pass

  @abstractmethod
  def get_info(self, i, param=None):
    pass

  @abstractmethod
  def get_number_experiments(self):
    pass

  @abstractmethod
  def get_number_experiments_ups(self):
    pass

  @abstractmethod
  def get_number_experiments_cps(self):
    pass

  @abstractmethod
  def get_name_for(self, i):
    pass

  @abstractmethod
  def get_info_for(self, i):
    pass

  @abstractmethod
  def get_num_lambdas(self):
    pass

  @abstractmethod
  def get_cost(self):
    pass

  @abstractmethod
  def get_cost_per_sec(self):
    pass
  
  def kill_all(self):
    for obj in cirrus_objs:
      obj.kill();
