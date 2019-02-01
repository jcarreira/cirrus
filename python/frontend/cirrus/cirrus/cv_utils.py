def create_test_train_sets(num_sets, k = 5):
    train_sets = []
    test_sets = []
    per_set = num_sets // k
    remainder = num_sets - (per_set * k)
    total_sets = [[i * per_set, (i + 1) * per_set - 1] for i in range(k)]
    total_sets[len(total_sets) - 1][1] += remainder
    print(total_sets)
    for i in range(len(total_sets)):
        train_sets += [total_sets[:i] + total_sets[i+1:]]
        test_sets += [(total_sets[i][0], total_sets[i][1])]
    return [train_sets, test_sets]
