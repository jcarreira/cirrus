import threading
import graph
import itertools
import cirrus
import time
from MLJob import MLJob
from GridSearch import GridSearch
from operator import itemgetter
from utils import *

class CrossValidation(MLJob):
  def __init__(self, num_sets, param_base, machines, task = GridSearch, k = 10, hyper_vars = [], hyper_params = []):
    super(CrossValidation, self).__init__(task)

    per_set = num_sets // k
    remaining = num_sets % k
    sets = [((i * per_set), (i + 1) * per_set - 1) for i in range(k - 1)]
    sets.append((((k - 1) * per_set), (k * per_set) - 1 + remaining))
    self.cirrus_objs = []
    self.k = k
    self.infos = []
    base_port = 1337
    for i in range(k):
      param_base['test_set'] = sets[i]
      param_base['train_set'] = sets[:i] + sets[i + 1:]
      self.cirrus_objs.append(self.task(task=cirrus.LogisticRegression,
                                             param_base=param_base,
                                             hyper_vars=hyper_vars,
                                             hyper_params=hyper_params,
                                             machines=machines,
                                             base_port=base_port))
      base_port += 2 * len(list(itertools.product(*hyper_params)))
      self.infos.append({'color': get_random_color()})
    self.loss_lst = [[] for _ in range(self.get_number_experiments())]
    self.num_threads = len(self.loss_lst)
  
  def run(self, UI=False):
    for obj in self.cirrus_objs:
      obj.run()
      time.sleep(15)
    self.start_threads()
    if UI:
      def ui_func(self):
        graph.bundle = self
        graph.app.run_server()
      self.ui_thread = threading.Thread(target=ui_func, args = (self, ))
      self.ui_thread.start()

  def get_number_experiments(self):
    return self.cirrus_objs[0].get_number_experiments()
  
  def get_number_experiments_ups(self):
    return self.cirrus_objs[0].get_number_experiments() * self.k

  def get_number_experiments_cps(self):
    return self.cirrus_objs[0].get_number_experiments() * self.k

  def get_info(self, i, param = None):
    out = self.infos[i]
    if param:
      return out[param]
    else:
      return out

  def start_threads(self):
    def custodian(cirrus_objs, i):
      while True:
        total_ttl = {}
        for cirrus_obj in cirrus_objs:
          loss_lst = cirrus_obj.loss_lst[i]
          for time_to_loss in loss_lst:
            rounded_time = int(round(time_to_loss[0]))
            if rounded_time in total_ttl:
              total_ttl[rounded_time].append(time_to_loss[1])
            else:
              total_ttl[rounded_time] = [time_to_loss[1]]
        avg_ttl = [(time, sum(total_ttl[time]) / len(total_ttl[time])) for time in total_ttl]
        self.loss_lst[i] = sorted(avg_ttl, key = itemgetter(0))

    for i in range(self.num_threads):
      p = threading.Thread(target=custodian, args=(self.cirrus_objs, i))
      p.start()

  def get_xs_for(self, i, metric="LOSS"):
    if metric == "LOSS":
      lst = self.loss_lst[i]
    elif metric == "UPS":
      x = i // self.cirrus_objs[0].get_number_experiments()
      y = i % self.cirrus_objs[0].get_number_experiments()
      lst = self.cirrus_objs[x].cirrus_objs[y].get_updates_per_second(fetch=False)
    else:
      x = i // self.cirrus_objs[0].get_number_experiments()
      y = i % self.cirrus_objs[0].get_number_experiments()
      lst = self.cirrus_objs[x].cirrus_objs[y].get_cost_per_second()
    return [item[0] for item in lst]

  def get_ys_for(self, i, metric="LOSS"):
    if metric == "LOSS":
      lst = self.loss_lst[i]
    elif metric == "UPS":
      x = i // self.cirrus_objs[0].get_number_experiments()
      y = i % self.cirrus_objs[0].get_number_experiments()
      lst = self.cirrus_objs[x].cirrus_objs[y].get_updates_per_second(fetch=False)
    else:
      x = i // self.cirrus_objs[0].get_number_experiments()
      y = i % self.cirrus_objs[0].get_number_experiments()
      lst = self.cirrus_objs[x].cirrus_objs[y].get_cost_per_second()
    return [item[1] for item in lst]

  def get_name_for(self, i):
    return "blah"

  def get_info_for(self, i):
    return "blah2"

  def get_num_lambdas(self):
    return 0
  
  def get_cost(self):
    return 0

  def get_cost_per_second(self):
    return 0


  
