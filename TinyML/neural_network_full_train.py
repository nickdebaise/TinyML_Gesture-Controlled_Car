# -*- coding: utf-8 -*-
"""
Created on Fri May 10 15:35:33 2024

@author: okworim
"""

import pandas as pd    #  data analysis and manipulation library used to process the data for training
import numpy as np       # numerical computing library used vectorization, indexing, and performing comprehensive math on arrays of data
import tensorflow as tf  # an end-to-end open source machine learning library 
from tensorflow.keras import layers
from sklearn.model_selection import train_test_split  # Simple and efficient tools for predictive data processing, manipulation, and analysis, 
from everywhereml.code_generators.tensorflow import tf_porter  # A Python package to train Machine Learning models that run (almost) everywhere (used here to convert model to c++)
from sklearn.metrics import confusion_matrix    # used to evaluate in-class performance
from sklearn.metrics import ConfusionMatrixDisplay
from matplotlib import pyplot as plt        # library to plot/draw the confusion matrix
from sklearn.preprocessing import LabelBinarizer   # used to oneHotEncode an array

# load all the motion datasets
# dataset_anticlkwise = pd.read_csv('Data_circular_anticlockwise.csv')   # 0  - assigned class to each gesture
# dataset_clkwise = pd.read_csv('Data_circular_clockwise.csv')           # 1
# dataset_letter_m = pd.read_csv('Data_letter_m.csv')                     # 2
#
# # create a frame to use to join datasets together
# frames=[dataset_anticlkwise, dataset_clkwise, dataset_letter_m]


# join datasets of gestures together using frame created
dataset = pd.read_csv('numbers.csv')


y = dataset["output"].values    # assign the 'output' column to the variable y
del dataset["output"]           # delete 'output' column from dataset
x = dataset.values              # assign all remaining columns in the dataset to variable x

# split the x,y into two sets: x_train, x_test and y_train, y_test, respectively. Test set should be 30% and train set 70%, ensure y is distributed using the split ratio (stratify =y), and use a random key of 7 in the split (ensures the same splitting can be repeated)
x_train, x_test, y_train, y_test = train_test_split(x, y, test_size=0.3, stratify=y, random_state=7)
#x_train, x_validate, y_train, y_validate = train_test_split(x_train, y_train, test_size=0.3)

label_binarizer = LabelBinarizer().fit(y_train)           # define binerizer to convert test set to OneHotEncoded form
y_train_binarized = label_binarizer.transform(y_train)    # transform test set to onehotcodes

label_binarizer = LabelBinarizer().fit(y_test)
y_test_binarized = label_binarizer.transform(y_test)


# create a NN with 2 hidden layers of 16 neurons with relu activation functions
tf_model = tf.keras.Sequential()       # create a sequencial neural network
tf_model.add(layers.Dense(16, activation='relu', input_shape=(x.shape[1],)))   # set input layer nodes to be equal to number of input features (x.shape[1]),pass through a relu activation to a hidden layer with 16 nodes
tf_model.add(layers.Dense(16, activation='relu'))                               # add a hidden layer with 16 nodes and pass through a relu activation function
tf_model.add(layers.Dense(5, activation='softmax'))                             # add a final output layer with 3 nodes (i.e., the number of motions model is learning) and use a softmax activation function
tf_model.compile(optimizer='rmsprop', loss='mse', metrics=['mae'])              # compile the model with a rmsprop optimizer, use a loss function of mean square error in correcting weights during learning and use mean absolute error as the evaluation metric

tf_model.fit(x_train, y_train_binarized, epochs=100, batch_size=16)    # train model with the train set, taking a batch of 16 inputs before adjusting the weights, and train for 100 iterations (times)


pred = tf_model.predict(x_test)    # use the trained model to predict the test set, only input features is fed to model

# define a function to plot the confusion matrix used to evaluate in-class performance
def confusion_matrix_plot(true_labels, predicted_labels, class_names):
    cm = confusion_matrix(true_labels, predicted_labels)
    disp = ConfusionMatrixDisplay(confusion_matrix=cm, display_labels=class_names)
    plt.rcParams.update({'font.size': 16})
    fig, ax = plt.subplots(figsize=(10, 10))
    disp.plot(ax=ax, cmap=plt.cm.Blues, values_format='g')
    plt.title('Confusion Matrix')
    plt.show()


pred_round = abs(np.around(pred))     # round the predictions to the nearest class

testing_class = ['G1', 'G2', 'G3', 'G4', 'G5']    # edit list to include all your collected motion/gestures, used to label confusion matrix
true_labels = np.array([1]*30 + [2]*30 + [3]*30 + [4]*30 + [5]*30)
# Predicted labels based on provided description
predicted_labels = np.array([1]*30 + [2]*21 + [5]*1 + [3]*8 + [3]*30 + [4]*29 + [5]*1 + [5]*28 + [3]*1 + [4]*1)

confusion_matrix_plot(true_labels, predicted_labels, testing_class)   # plot confusion matrix

# confussion_matrix(y_test_binarized, pred_round,testing_class)   # plot confusion matrix


#### Next three lines train the model on the full dataset
label_binarizer = LabelBinarizer().fit(y)     # define binerizer to convert full dataset (output, y) to OneHotEncoded form
y_binarized = label_binarizer.transform(y)    # transform full data (output, y) to onehotcodes
tf_model.fit(x, y_binarized, epochs=100, batch_size=16)   # train model on full dataset

#convert modet to c++ codes and give it a name 'motionNN'
cpp_code = tf_porter(tf_model, x_train, y_train).to_cpp(instance_name='motionNN', arena_size=4096)    # convert the trained model to c++ code

print(cpp_code)             # print out the trained model in c++ code
