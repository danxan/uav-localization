import keras
from keras.models import Sequential
from keras.layers import Activation, Flatten, Dense, Conv2D, Flatten, MaxPooling2D, AveragePooling2D


from DataProcessor import *


class ConvNeuralNet:


	def __init__(self, X_train, X_test, y_train, y_test, img_shape=[64,64,3], num_classes=2):
		self.X_train = X_train
		self.X_test = X_test
		self.y_train = y_train
		self.y_test = y_test

		self.img_shape = img_shape
		self.num_classes = num_classes


	def create_model(self):
		self.model = Sequential()#add model layers
		
		self.model.add(Conv2D(filters=6, kernel_size=(3, 3), activation='relu', input_shape=(64,64,3)))
		self.model.add(AveragePooling2D())

		self.model.add(Conv2D(filters=16, kernel_size=(3, 3), activation='relu'))
		self.model.add(AveragePooling2D())

		self.model.add(Flatten())

		self.model.add(Dense(units=120, activation='relu'))

		self.model.add(Dense(units=84, activation='relu'))

		self.model.add(Dense(units=2, activation = 'softmax'))

		#compile model using accuracy to measure model performance
		adam = keras.optimizers.Adam(lr=0.0002)
		self.model.compile(optimizer=adam, loss="categorical_crossentropy", metrics=["accuracy"])



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
	cnn.model.fit(cnn.X_train, cnn.y_train, batch_size=10, validation_data=(cnn.X_test, cnn.y_test), epochs=100)

	# classify images
	print(cnn.model.predict(cnn.X_test[:10]))
