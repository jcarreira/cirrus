# Logistic Regression on Diabetes Dataset
from random import seed
from random import randrange
from csv import reader
from math import exp
import time
import sys

#Id,Label,I1,I2,I3,I4,I5,I6,I7,I8,I9,I10,I11,I12,I13,C1,C2,C3,C4,C5,C6,C7,C8,C9,C10,C11,C12,C13,C14,C15,C16,C17,C18,C19,C20,C21,C22,C23,C24,C25,C26
# integer features: 2-14 inclusive
# cat features: 15-40

# result is a list of tuples (label, list of tuples (index, value))
# Load a CSV file
# we treat cat values as hexadecimal and transform them into ints
# we set missing values to -1
def load_csv_sparse_tab(filename):
    dataset = list()
    with open(filename, 'r') as file:
        csv_reader = reader(file)
        r = next(csv_reader)
        for row in csv_reader:
            #print("row:", row)
            tuples_list = list()

            # add numerical values to list
            for i in range(2, 15):
                if row[i] == '': # if value is missing we set -1
                    row[i] = "-1"
                tuples_list.append([i-2, float(row[i])])

            for i in range(15, len(row)):
                if row[i] == '':
                    row[i]=-1
                else:
                    row[i]=int(row[i], 16)
                tuples_list.append([i-2, float(row[i])])

            row_entry = (float(row[1]), tuples_list)
            dataset.append(row_entry)
    return dataset

boundaries = [1.5**j - 0.51 for j in range(40)]
def find_bucket(value):
    global boundaries

    i = 0
    while i < len(boundaries) and value > boundaries[i]:
        i += 1
    return i

def count_unique_items(frequencies):
    total_unique_items = 0
    for i in range(0, len(frequencies)):
        total_unique_items += len(frequencies[i])
        print("Column i: ", i, " has length: " , len(frequencies[i]))
    return total_unique_items

def bucketize_integer_features(dataset):
    for sample in dataset:
        # go to all integer features
        #print("sample:", sample)
        for i in range(0, 13):
            features = sample[1]
            pair = features[i]

            # find bucket for this value
            # missing values previously assigned -1 get bucket 0
            pair[1] = find_bucket(pair[1])

def calc_col_frequencies(frequencies, dataset):
    print("Computing frequencies of each column")
    sample_len = len(dataset[0][1])
    for i in range(0, sample_len):
        print("frequencies of column: ", i)
        col_frequencies = dict()
        for sample in dataset:
            features = sample[1]
            pair = features[i]
            value = pair[1]

            if value not in col_frequencies:
                col_frequencies[value] = 1
            else:
                col_frequencies[value] += 1
        frequencies.append(col_frequencies)


# here we do:
# 1. integerize features 
# 2. ignore features whose values appear <15 times in the col
# 3. bucketize integer features
base_index = 0
def preprocess(dataset):
    global base_index

    #bucketize_integer_features(dataset)

    # compute frequencies
    print("length of a sample: ", len(dataset[0][1]))
    sample_len = len(dataset[0][1])
    frequencies = []

    calc_col_frequencies(frequencies, dataset)

    print("total_unique_items: ", count_unique_items(frequencies))

   
    print("Marking cat features that appear less than 15 times")
    for i in range(13, sample_len):
        print("Column i:", i)
        for sample in dataset:
            features = sample[1]
            pair = features[i]
            value = pair[1]

            if frequencies[i][value] < 15:
                #print "value: ", value, " freq: ", frequencies[i][value]
                #print "popping: ", features[i]
                pair[1] = -sys.maxint - 1 # mark this for deletion later

        #for sample in dataset:
        #    features = sample[1]
        #    # ignore pairs that we marked to remove
        #    features = [pair for pair in features if pair[0] != -1]
    
    # now we expand each feature
    for i in range(0, 13):
        for sample in dataset:
            features = sample[1]
            pair = features[i]
            value = pair[1]

            pair[0] = base_index + find_bucket(value)
            pair[1] = 1
        base_index += len(boundaries) + 1
    
    for i in range(0, 13):
        for sample in dataset:
            features = sample[1]
            pair = features[i]
            value = pair[1]

            if value == -1:
                print "Error"
                exit(-1)
    
    for i in range(13, sample_len):
        feature_id = 0
        value_to_featureid = dict()
        for sample in dataset:
            features = sample[1]
            pair = features[i]
            value = pair[1]

            if value not in value_to_featureid:
                value_to_featureid[value] = feature_id
                feature_id += 1

            pair[0] = base_index + value_to_featureid[value]
            pair[1] = 1
        base_index += feature_id
        
    for sample in dataset:
        features = sample[1]
        features.append([base_index, 1]) # add bias
    
    base_index += 1
    print("last base_index: ", base_index)

    for i in range(13, sample_len):
        for sample in dataset:
            features = sample[1]
            for feature in features:
                if feature[1] == -sys.maxint - 1:
                    features.remove(feature)
            assert(len(features) > 0)


# Convert string column to float
def str_column_to_float(dataset, column):
    for row in dataset:
        if row[column] == '':
            row[column] = "0"
        row[column] = float(row[column].strip())

# Calculate accuracy percentage
def accuracy_metric(actual, predicted):
    correct = 0
    for i in range(len(actual)):
        if actual[i] == predicted[i]:
            correct += 1
    return correct / float(len(actual)) * 100.0

# Split a dataset into k folds
def cross_validation_split(dataset, n_folds):
    dataset_split = list()
    dataset_copy = list(dataset)
    fold_size = int(len(dataset) / n_folds)
    for i in range(n_folds):
        fold = list()
        while len(fold) < fold_size:
            index = randrange(len(dataset_copy))
            fold.append(dataset_copy.pop(index))
        dataset_split.append(fold)
    return dataset_split

# Evaluate an algorithm using a cross validation split
def evaluate_algorithm(dataset, algorithm, n_folds, *args):
    print("evaluate_algorithm")
    folds = cross_validation_split(dataset, n_folds)
    scores = list()
    for fold in folds:
        start_time = time.time()
        print "Evaluating new fold (out of %d)" % len(folds)
        train_set = list(folds)
        train_set.remove(fold)
        train_set = sum(train_set, [])
        test_set = list()
        for row in fold:
            row_copy = list(row)
            test_set.append(row_copy)
            #row_copy[-1] = None
        predicted = algorithm(train_set, test_set, *args)
        actual = [row[0] for row in fold]
        accuracy = accuracy_metric(actual, predicted)
        print("Acc:", accuracy)
        scores.append(accuracy)
        print("Elapsed time fold: ", time.time() - start_time)
    return scores

# Make a prediction with coefficients
def predict(row_values, coefficients):
    #print("row: ", row)
    #print("coefficients: ", coefficients)
    yhat = coefficients[0]
    for i in range(len(row_values)):
        index = row_values[i][0]
        value = row_values[i][1]

        yhat += coefficients[index] * value
    return 1.0 / (1.0 + exp(-yhat))

# Estimate logistic regression coefficients using stochastic gradient descent
def coefficients_sgd(train, l_rate, n_epoch):
    global base_index
    coef = [0.0 for i in range(base_index)]
    for epoch in range(n_epoch):
        for row in train:
            label = row[0]
            values = row[1]

            yhat = predict(values, coef)
            error = label - yhat
            #print "yhat: ", yhat
            #print "label: ", label
            #coef[0] = coef[0] + l_rate * error * yhat * (1.0 - yhat)
            for i in range(len(values)):
                index = values[i][0]
                value = values[i][1]
                coef[index] = coef[index] + l_rate * error * yhat * (1.0 - yhat) * value

    return coef

def print_dataset(dataset):
    print "Printing dataset"
    for sample in dataset:
        features = sample[1]
        print features


# Linear Regression Algorithm With Stochastic Gradient Descent
def logistic_regression(train, test, l_rate, n_epoch):
    predictions = list()
    print "logistic_regression n_epoch: ", n_epoch
    print "logistic_regression train len: ", len(train)
    coef = coefficients_sgd(train, l_rate, n_epoch)
    for row in test:
        #print("test row: ", row)
        label = row[0]
        values = row[1]

        yhat = predict(values, coef)
        yhat = round(yhat)
        predictions.append(yhat)
    return(predictions)


seed(1)
# load and prepare data
filename = "/home/joao/data/criteo_kaggle/train.csv_1M"
#filename = "/home/joao/data/criteo_kaggle/train.csv_10K"

print("Loading dataset")
dataset = load_csv_sparse_tab(filename)
print("Preprocessing dataset")
#print_dataset(dataset)
preprocess(dataset)
#print_dataset(dataset)

# evaluate algorithm
n_folds = 2
l_rate = 0.001
n_epoch = 20

print("Evaluating algorithm")
scores = evaluate_algorithm(dataset, logistic_regression, n_folds, l_rate, n_epoch)
print('Scores: %s' % scores)
print('Mean Accuracy: %.3f%%' % (sum(scores)/float(len(scores))))
