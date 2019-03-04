import time

# Given a running ParameterServer object, this function checks
# if the accuracy of the model increases above acc_threshold within
# the timeout.
def check_acc(server, acc_threshold=0.73, timeout=60):
    total_line = ""
    for line in server.error_output().split("\n")[-10:]:
        total_line = total_line + " " + line
    timeout = time.time() + timeout
    recent_acc = -0.5
    while True:
        time.sleep(1)
        if time.time() > timeout:
            break
        words = total_line.split(" ")
        accuracy = False
        for word in words:
            if accuracy:
                recent_acc = float(word)
                accuracy = False
            else:
                if word == "Accuracy:":
                    accuracy = True
    if recent_acc >= acc_threshold:
        print "Test passed"
    elif recent_acc < 0:
        print "Test failed. There was an error running ErrorSparseTask \
                or wait for about 30 seconds for ErrorSparseTask."
    else:
        print "Test failed. Accuracy not high enough." + "(" + str(recent_acc) + ")"
