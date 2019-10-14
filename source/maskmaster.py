import numpy as np
import scipy.io as sio
import cv2

'''
NOTHING IS TESTED. Yet to implement test functions.
'''

class Maskmaster():
    def read_image(input_filename):
        '''
        Reads the given input_filename into a numpy array.
        Args: string
        Returns: numpy array
        '''
        return cv2.imread(input_filename)

    def write_slices(output_dir, imarr):
        '''
        Writes the given array to a binary file of the given filename.
        Args: string, numpy array
        '''
        #TODO: check if directory exists, and handle it.
        #TODO: Check how easy it is to separate between the slices.
        sio.save(output_dir, imarr)


    def slice_image(output_dir, imarr, dims):
        '''
        Slices the image in the given file descriptor into patches of the given 2d-dimensionality.
        Stores the slices into files in the given directory by use of write_slices().
        Args: string, numpy array, tuple of ints
        Returns: A list of file descriptors
        '''
        slices = [im[x:x+M,y:y+N] for x in range(0,im.shape[0],dims[0]) for y in range(0,im.shape[1],dims[1])]
        write_slices(output_dir, imarr)
        return slices


    def feed_net(input_dir, network, net_pred_dir):
        '''
        Feeds a the input directory to the given network object. Stores a collection of the network predictions to file.
        Args: string, network object, string
        Returns: collection of network predictions
        '''
        #TODO: Write a common interface for the networks...

    def make_mask(*pred_collection, mask_arr_dir):
        '''
        Assumes the input predictions is a list of one-hot vectors. Returns a n-dimensional array that is the mask.
        Args: List, string
        Returns: n-d array, where n is the number of classes+long.+lat.
        '''
#        mask_arr = np.zeros(shape=[
#        for v in pred_collection:











