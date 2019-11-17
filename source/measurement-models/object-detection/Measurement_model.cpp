#include <vector>
#include <string>
#include <math.h>
#include <bits/stdc++.h>

using namespace std;

#define MAP_WIDTH 4800
#define MAP_HEIGHT 4800

#define MPPX 0.243 // dependent on spatial resolution of sattelite image
#define ARWIDTH 1.87 // 1.87 is dependent on aspect ratio
#define ARHEIGHT 1.05 // 1.05 is dependent on aspect ratio

#define PI 3.14159265
/**
 * Assumes two vectors of integers, uav and particle,
 * where each index is assigned to a class,
 * determined by the string vector dict.
 *
 * Args:
 *      uav: vector of object occurencies for each class
 *      particle: vector of object occurencies for each class
 *      dict: vector of class names
 *      num_classes: number of classes
 * Returns:
 *      Sum of squares.
 */
int compare_lists(vector<int> uav, vector<int> particle, vector<string> dict, int num_classes){
    int sum = 0;
    for(int i = 0; i < num_classes; i++){
        // Square the sum to avoid negative numbers.
        // No squareroot because of computational intensity.
        sum += (uav[0] - particle[0])*(uav[0] - particle[0]);
    }
    return sum;
}

/**
 * Computes a list of object occurencies for a particle+
 * based on a precomputed matrix of occurencies for the global map.
 *
 * Args
 *      matrix: Each cell contains an vector of integers,
 *              that is the number of object occurencies.
 *      pose:   [x,y,z, theta], where theta is the angle offset, from north.
 */
int list_from_matrix(int*** matrix, int* pose){
    return 0;
}

/**
 * Converts x,y from pose to 2D matrix coordinates.
 * Note: mshape should be dependent on patch-size earlier in the program.
 *
 * Args:
 *      pose: vector containing the particle pose
 *          pose[0]: x element in [0,MAP_WIDTH)
 *          pose[1]: y element in [0, MAP_HEIGHT)
 *          pose[2]: z is a float. 0.49 corresponds to 93m alt. and is linear
 *          pose[3]: theta is the angle of orientation in the xy plane.
 *      mshape: vector containing the shape of the 2D matrix.
 *          mshape[0]: number of rows
 *          mshape[1]: number of columns
 * Returns:
 *      vector with 2D matrix coordinates ( [row#, col#] )
 */
vector<int> pose_to_mcoords(vector<int> pose, int rows, int cols){
    vector<int> ret;

    int pxpw = MAP_WIDTH/rows; // Pixels per Patch-Width
    ret[0] = pose[0]/pxpw;

    int pxph = MAP_HEIGHT/cols; // Pixels per Patch-Height
    ret[1] = pose[1]/pxph;

    return ret;
}

/**
 * Computes a sum of all the patches included
 * in the virtual view of the particle.
 * Each element of the matrix is a patch,
 * which contains only one value.
 */
int sum_from_cross_simple(vector< vector<int> > matrix, vector<int> pose){
    // Pseudocode:
    // Find patch of the center.
    int rows = matrix.size();
    int cols = matrix[0].size();
    int pxpw = MAP_WIDTH/rows; // Pixels per Patch-Width
    int pxph = MAP_HEIGHT/cols; // Pixels per Patch-Height

    int rowcoord = pose[0]/pxpw; // The row location of particle
    int colcoord = pose[1]/pxph; // The column location of particle

    // Find width and height of viewbox, in pixels.
    //int altitude = convert_to_altitude(pose[2]);
    int altitude = 93;
    int vb_w_px = altitude*ARWIDTH/MPPX;
    int vb_h_px = altitude*ARHEIGHT/MPPX;

    // Rotate viewbox to align with global frame, inside original boundaries (shrink it)
    // Assuming pose[3] is angle represented in degrees
    double rad = pose[3]*PI/180;
    int tmp1 = vb_w_px*sin(rad);
    int tmp2 = vb_w_px*cos(rad);
    int tmp3 = vb_h_px*sin(rad);
    int tmp4 = vb_h_px*cos(rad);
    int avb_w_px;
    int avb_h_px;
    if(tmp2 > tmp3){
        avb_w_px = tmp2;
    }else{
        avb_h_px = tmp3;
    }
    if(tmp1 > tmp4){
        avb_w_px = tmp1;
    }else{
        avb_h_px = tmp4;
    }

    int num_patches_w = avb_w_px/pxpw;
    int num_patches_h = avb_h_px/pxph;

    // TODO: Handle case when particle is near border of map...
    // Get values from matrix, around particle coordinates
    int sum = 0;
    int start_w = rowcoord - num_patches_w/2; // TODO? Handle float sitations?
    for(int i = start_w; i < num_patches_w; i++){
        sum += matrix[i][colcoord];
    }
    int start_h = colcoord - num_patches_h/2;
    for(int i = start_h; i < num_patches_h; i++){
        sum += matrix[rowcoord][i];
    }

    return sum;
}

int sum_from_box_simple(vector< vector<int> > matrix, vector<int> pose){
    // TODO: (Get the diagonal cross)
    //      (Make a function to find all corners,
    //      then "read" everything inside the extended box,
    //      then check whether 2/4 points around the center of the read patch
    //      is inside or outside the line drawn between the two nearest corners of the box.)
    // Get the cross that is aligned with the particle orientation.
    //
    // Get the cross that is aligned with the global coordinate system.

    return 0;
}

int width_in_num_patches(int altitude, int patch_width){
    int width = ARWIDTH*altitude;

    return  width/(MPPX*patch_width);
}

int height_in_num_patches(int altitude, int patch_height){
    int height = ARHEIGHT*altitude;

    return  height/(MPPX*patch_height);
}

int main(){
    return 0;
}

