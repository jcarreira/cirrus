def create_test_train_sets(num_sets, k = 5):
    train_sets = []
    test_sets = []
    per_set = num_sets // k
    remainder = num_sets - (per_set * k)
    total_sets = [[i * per_set, (i + 1) * per_set - 1] for i in range(k)]
    total_sets[len(total_sets) - 1][1] += remainder
    for i in range(len(total_sets)):
        train_sets += [total_sets[:i] + total_sets[i+1:]]
        test_sets += [(total_sets[i][0], total_sets[i][1])]
    return [train_sets, test_sets]

def get_cv_acc(cirrus_objs):
    total = 0
    for o in cirrus_objs:
        total += get_acc(o)
    return float(total) / float(len(cirrus_objs))

def get_acc(o):
    l = ""
    for line in o.ps.error_output().split("\n")[-20:]:
        l = l + " " + line
    words = l.split(" ")
    accuracy = False
    recent_acc = -0.5
    for word in words:
        if accuracy:
            recent_acc = float(word)
            accuracy = False
        else:
            if word == "Accuracy:":
                accuracy = True
    if recent_acc < 0:
        raise Exception('Accuracy is not defined yet. Please wait a little and run again.')
    return recent_acc

    
