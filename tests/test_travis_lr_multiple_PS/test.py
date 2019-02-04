import subprocess
import time
import sys
import os
import signal

ps_command_1 = ["./tests/test_travis_lr_multiple_PS/test_ps", "1037"]
ps_command_2 = ["./tests/test_travis_lr_multiple_PS/test_ps", "1039"]
worker_command = ["./tests/test_travis_lr_multiple_PS/worker"]
error_command = ["./tests/test_travis_lr_multiple_PS/error"]

try:
  ps1 = subprocess.Popen(ps_command_1, stderr=subprocess.STDOUT)
  ps2 = subprocess.Popen(ps_command_2, stderr=subprocess.STDOUT)
  time.sleep(5)
  p2 = subprocess.Popen(worker_command, stderr=subprocess.STDOUT)
  p2.wait()
  p3 = subprocess.Popen(error_command, stderr=subprocess.STDOUT)
except:
  sys.exit(-1)

# Check Error
while p3.poll() is None:
  time.sleep(5)

# Check PS1
if ps1.poll() is not None:
  if ps1.returncode != 0:
    print(ps1.returncode)
    print("PS crashed")
    sys.exit(ps1.returncode)

# Check PS1
if ps2.poll() is not None:
  if ps2.returncode != 0:
    print(ps2.returncode)
    print("PS crashed")
    sys.exit(ps2.returncode)

# Chekc the worker
if p2.poll() is not None:
  if p2.returncode != 0:
    p1.terminate()
    print("worker crashed")
    sys.exit(p2.returncode)

# Terminate PS
ps1.terminate();
ps2.terminate();

# Get return code from error task
return_code = p3.returncode
print(return_code)
sys.exit(return_code)
