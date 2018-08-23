from abc import ABC, abstractmethod

class MLJob(ABC):
  def __init__(self, other_job = None):
    self.loss_lst = []
    self.cirrus_objs = []
    self.other_job = other_job
    
  @abstractmethod
  def get_xs_for(self, i, metric = "LOSS") :
    pass

  @abstractmethod
  def get_ys_for(self, i, metric = "LOSS"):
    pass

  @abstractmethod
  def run(self, UI=False):
    pass
  
  def kill_all(self):
    for obj in cirrus_objs:
      obj.kill();
