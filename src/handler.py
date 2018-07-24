import os
import subprocess
import threading
from time import sleep
import time
import sys
import socket

CHARS_TO_PRINT = 100
TIMEOUT_TIME   = 60
CONN_TIMEOUT_SECS = 10

#CONFIG_PATH = "configs/criteo_kaggle_19b_random.cfg"
# CONFIG_PATH = "configs/criteo_kaggle.cfg"
CONFIG_PATH = "./lda.cfg"

libdir = os.path.join(os.getcwd(), 'local', 'lib')

def launch_ps(num_workers, num_task, ps_ip, ps_port, use_softmax):

    command =  \
      "./parameter_server --config {} --nworkers {}" \
      " --rank {} --ps_ip {} --ps_port {}" \
      .format(CONFIG_PATH, num_workers, num_task, ps_ip, ps_port)

    print("Command: ", command)

    return
    process = subprocess.Popen(command.split(), stdout=subprocess.PIPE)

    now = time.time()
    output = ""
    for c in iter(lambda: process.stdout.read(CHARS_TO_PRINT), ''):
        output = c
        for c in iter(lambda: process.stdout.read(1), ''):
            output += c
            if c == '\n':
               print(output)
               output = ""
               break

    for c in iter(lambda: process.stdout.read(1), ''):
        output += c
    print(output)

    return ''.join(output)

def cpu_benchmark():
    command =  './bin_worker'

    output = ""
    print("Command: ", command)
    process = subprocess.Popen(command.split(), stdout=subprocess.PIPE)

    now = time.time()
    for c in iter(lambda: process.stdout.read(CHARS_TO_PRINT), ''):
        diff = time.time() - now
        if diff > TIMEOUT_TIME:
            break
        output += c
    print(output)

    return ''.join(output)

def iperf_benchmark():
    command =  './iperf3 -c 172.31.12.171 -t 5'

    output = ""
    print("Command: ", command)
    process = subprocess.Popen(command.split(), stdout=subprocess.PIPE)

    now = time.time()
    for c in iter(lambda: process.stdout.read(CHARS_TO_PRINT), ''):
        diff = time.time() - now
        if diff > TIMEOUT_TIME:
            break
        output += c
    print(output)

    return ''.join(output)

def redis_benchmark():
    command =  './redis_benchmark_lambdas'

    output = ""
    print("Command: ", command)
    process = subprocess.Popen(command.split(), stdout=subprocess.PIPE)

    for c in iter(lambda: process.stdout.read(CHARS_TO_PRINT), ''):
        output += c
    print(output)

    return ''.join(output)

def redis_benchmark_pubsub():
    command =  './redis_sender'

    output = ""
    print("Command: ", command)
    process = subprocess.Popen(command.split(), stdout=subprocess.PIPE)

    for c in iter(lambda: process.stdout.read(CHARS_TO_PRINT), ''):
        output += c
    print(output)

    return ''.join(output)

def register_task_id(ps_ip, ps_port, task_id):
    print "Registering ps_ip: ", ps_ip, \
          " ps_port: ", ps_port,\
          " task_id: ", task_id

    REGISTER_TASK = '\x08\x00\x00\x00'

    try:
        clientsocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        clientsocket.settimeout(CONN_TIMEOUT_SECS)
        clientsocket.connect((ps_ip, ps_port))

        # tell the parameter server we want to regis
        clientsocket.send(REGISTER_TASK)
        # check the success of this
        s = clientsocket.recv(32)

        success = struct.unpack("I", s)[0]
    except:
        raise Exception('Error registering with the parameter server')

    return success > 0

def handler(event, context):
    results = []

    print("HELLO WORLD")
    print("event: ", event)
    num_task = event['num_task']
    num_workers = event['num_workers']

    ps_ip = ''
    ps_port = ''
    if 'ps_ip' in event:
        ps_ip = event['ps_ip']

        if 'ps_port' not in event:
            raise 'ps_port not found'

        ps_port = event['ps_port']

        if 'task_id' in event:
            if register_task_id(ps_ip, ps_port, event['task_id']) == False:
                return # terminate task because this is a repeated lambda

    use_softmax = False

    print("num_task: ", num_task)
    print("num_workers: ", num_workers)
    print("use_softmax: ", use_softmax)

    # use_softmax: false for criteo, true for MNIST
    results = launch_ps(num_workers, num_task, ps_ip, ps_port, use_softmax)

    return []

handler({"num_task":"5", "num_workers":"2", "ps_ip": "172.31.12.171", \
       "ps_port": 1337, 'task_id':"1"}, 0)
