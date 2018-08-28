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
  def get_number_experiments(self):
    pass

  @abstractmethod
  def get_number_experiments_ups(self):
    pass

  @abstractmethod
  def get_number_experiments_cps(self):
    pass
  
  def kill_all(self):
    for obj in cirrus_objs:
      obj.kill();
