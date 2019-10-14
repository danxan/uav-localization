import keras
import numpy as np
import pandas as pd
from keras.applications import resnet50
from keras import models
from keras import layers
from keras import optimizers
import cv2
from sklearn.metrics import classification_report, confusion_matrix
import matplotlib.pyplot as plt
import os
from matplotlib import pyplot as plt
import numpy as np
from PIL import Image
from keras.callbacks import ModelCheckpoint

from DataProcessor import *


repo_path = os.path.dirname(os.getcwd())
eurosat_path = os.path.join(repo_path,"eurosat_reduced")

processor = DataProcessor(eurosat_path)

dataset = processor.load_eurosat()
img_classes = processor.classes

X_train, X_test, y_train, y_test = processor.prepare_data(dataset)

resnet_model = resnet50.ResNet50(weights='imagenet', include_top=False, input_shape=(64, 64, 3))
model = models.Sequential()
model.add(resnet_model)

# Adding new Layers
model.add(layers.Flatten())
model.add(layers.Dense(1024, activation='relu'))
model.add(layers.Dropout(0.5))
model.add(layers.Dense(2, activation='softmax'))

'''
from keras.preprocessing.image import ImageDataGenerator
train_datagen = ImageDataGenerator( 
        rescale=1./255,
        shear_range=0.2,             
        zoom_range=0.2,              
        horizontal_flip=True)
test_datagen = ImageDataGenerator(rescale=1./255)
path = ""
os.chdir(path)
training_set = train_datagen.flow_from_directory(
        'TrainingSet',
         target_size=(64,64),
         batch_size=32,
         class_mode='categorical')'''

model.compile(loss='categorical_crossentropy',
              optimizer=optimizers.RMSprop(lr=1e-4),
              metrics=['acc'])

# Train the model
history = model.fit(X_train, y_train, batch_size=32, validation_data=(X_test, y_test), epochs=100)

# classify images
print(model.predict(cnn.X_test[:10]))

