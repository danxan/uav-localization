from sklearn.model_selection import train_test_split
import numpy as np
import os
import cv2

class DataProcessor:
	'''
	Module containing dataset shaping and processing functions.
	Currently aimed at processing EuroSat dataset only.
	'''

	def __init__(self, dataset_path):
		# set path to dataset
		self.dataset_path = dataset_path


	def load_eurosat(self):
		'''
		Store class names of image dataset,
		load dataset as numpy array of
		shape: [classes, elements, img_height, img_width, channels]
		'''

		# store dataset
		eurosat = []
		class_list = []

		# walk each image class directory
		for root, dirs, files in os.walk(self.dataset_path):
			for name in dirs:
				class_list.append(name)
				class_path = os.path.join(root, name)


				# store all images of same class
				img_list = []
				for class_root, class_dirs, class_files in os.walk(class_path):
					for img_name in class_files:
						img_path = os.path.join(class_path, img_name)
						img_list.append(cv2.imread(img_path))

				eurosat.append(np.array(img_list))

		self.classes = class_list
		dataset = np.array(eurosat)
		return dataset


	def prepare_data(self, dataset):
		'''
		Reshape dataset into a column vector of images
		and produce a one-hot encoded array of corresponding labels.
		Then, split data into training and testing sets.

		Args:
			dataset: reshape this dataset

		Returns:
				X_train: column vector of images for training
				 X_test: column vector of images for testing
				y_train: vector of labels for X_train
				 y_test: vector of labels for X_test
		'''

		# make labels
		labels = []
		for class_number in range(dataset.shape[0]):
			labels.append(np.ones([dataset.shape[1]])*class_number)
		labels = np.array(labels).T

		# reshape image set and labels
		X = np.vstack((dataset[:]))
		y = np.hstack((labels[:]))
		y_onehot = np.zeros([y.shape[0], dataset.shape[0]])
		for i in range(y.shape[0]):
			y_onehot[i, int(y[i])] = 1

		# split data
		X_train, X_test, y_train, y_test = train_test_split(X, y_onehot, test_size=0.2)
		return X_train, X_test, y_train, y_test



if __name__=="__main__":
	'''
	Process reduced eurosat dataset.
	'''
	repo_path = os.path.dirname(os.getcwd())
	eurosat_path = os.path.join(repo_path,"eurosat_reduced")

	processor = DataProcessor(eurosat_path)

	dataset = processor.load_eurosat()
	img_classes = processor.classes

	X_train, X_test, y_train, y_test = processor.prepare_data(dataset)