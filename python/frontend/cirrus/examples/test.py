import time

from cirrus import instance, automate, lr

def test(server, acc_threshold = 0.73, timeout = 60):
    l = ""
    for line in server.error_output().split("\n")[-10:]:
        l = l + " " + line
    timeout = time.time() + timeout
    recent_acc = -0.5
    while True:
        time.sleep(1)
        if time.time() > timeout:
            break
        words = l.split(" ")
        accuracy = False
        for word in words:
            if accuracy:
                recent_acc = float(word)
                accuracy = False
            else:
                if word == "Accuracy:":
                    accuracy = True
    if recent_acc >= 0.72:
        print("Test passed")
    elif recent_acc < 0:
        print("Test failed. There was an error running ErrorSparseTask or wait for about 30 seconds for ErrorSparseTask.")
    else:
        print("Test failed. Accuracy not high enough." + "(" + str(recent_acc) + ")")