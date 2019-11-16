#include <bits/stdc++.h>
using namespace std;

/**
 * Assumes two arrays of integers, uav and particle,
 * where each index is assigned to a class,
 * determined by the string array dict.
 *
 * Args:
 *      uav: array of object occurencies for each class
 *      particle: array of object occurencies for each class
 *      dict: array of class names
 *      num_classes: number of classes
 * Returns:
 *      Sum of squares.
*/
int compare_lists(int* uav, int* particle, string* dict, int num_classes){
    int sum = 0;
    for(int i = 0; i < num_classes; i++){
        // Square the sum to avoid negative numbers.
        // No squareroot because of computational intensity.
        sum += (uav[0] - particle[0])*(uav[0] - particle[0]);
    }
    return sum;
}

/**
 * Computes a list of object occurencies for a particles virtual view,
 * based on a precomputed matrix of occurencies for the global map.
 *
 * Args:
 *      matrix: Each cell contains an array of integers,
 *              that is the number of object occurencies.
 *      pose:   [x,y,z, theta], where theta is the angle offset, from north.
*/
int list_from_matrix(int*** matrix, int* pose){

}

int width_in_num_patches(int altitude, int patch_width){
    width = 1.87*altitude; // 1.87 is dependent on aspect ratio
    mppx = 0.243; // dependent on spatial resolution of sattelite image

    return  width/(mppx*patch_width)
}

int height_in_num_patches(int altitude, int patch_height){
    height = 1.05*altitude; // 1.05 is dependent on aspect ratio
    mppx = 0.243; // dependent on spatial resolution of sattelite image

    return  height/(mppx*patch_width)
}

int main(){
    return 0;
}

