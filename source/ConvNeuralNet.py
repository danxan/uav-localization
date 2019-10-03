from keras.models import Sequential
from keras.layers import Dense, Conv2D, Flatten

from DataProcessor import *


class ConvNeuralNet:


	def __init__(self, X_train, X_test, y_train, y_test, img_shape=[64,64,3], num_classes=4):
		self.X_train = X_train
		self.X_test = X_test
		self.y_train = y_train
		self.y_test = y_test

		self.img_shape = img_shape
		self.num_classes = num_classes


	def create_model(self):
		self.model = Sequential()#add model layers
		self.model.add(Conv2D(64, kernel_size=3, activation="relu",
						 input_shape=(self.img_shape[0], self.img_shape[1], self.img_shape[2])))
		self.model.add(Conv2D(32, kernel_size=3, activation="relu"))
		self.model.add(Flatten())
		self.model.add(Dense(self.num_classes, activation="softmax"))

		#compile model using accuracy to measure model performance
		self.model.compile(optimizer="adam", loss="categorical_crossentropy", metrics=["accuracy"])



if __name__ == "__main__":
	# load and prepare eurosat data
	repo_path = os.path.dirname(os.getcwd())
	eurosat_path = os.path.join(repo_path,"eurosat_reduced")

	processor = DataProcessor(eurosat_path)

	dataset = processor.load_eurosat()
	img_classes = processor.classes

	X_train, X_test, y_train, y_test = processor.prepare_data(dataset)

	# create neural net
	cnn = ConvNeuralNet(X_train, X_test, y_train, y_test)
	cnn.create_model()

	# train neural net
	cnn.model.fit(cnn.X_train, cnn.y_train, validation_data=(cnn.X_test, cnn.y_test), epochs=3)

	# classify images
	print(cnn.model.predict(cnn.X_test[:10]))
