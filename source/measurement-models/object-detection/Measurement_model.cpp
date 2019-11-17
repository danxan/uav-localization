#include <vector>
#include <string>
#include <math.h>
#include <bits/stdc++.h>
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"

#include "tmp.h"
using namespace std;
using namespace cv;

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

class Measurement_model{
    private:
        int num_rows;
        int num_cols;
        int patch_width;
        int patch_height;
        vector<int>** matrix;

        int width_conversion(int patch_width)  //patch size is in pixels
        {
            int i = patch_width;
            while((i != MAP_WIDTH) && (MAP_WIDTH % i != 0))
            {
                i++;
            }
            int j = patch_width;
            while((j != 0) && (MAP_WIDTH % j != 0))
            {
                j--;
            }

            if(patch_width - j < i - patch_width)
            {
                return j;
            }
            else
                return i;
        }

        int height_conversion(int patch_height)  //patch size is in pixels
        {
            int i = patch_height;
            while((i != MAP_HEIGHT) && (MAP_HEIGHT % i != 0))
            {
                i++;
            }
            int j = patch_height;
            while((j != 0) && (MAP_HEIGHT % j != 0))
            {
                j--;
            }

            if(patch_height - j < i - patch_height)
            {
                return j;
            }
            else
                return i;
        }

        vector<int> add_int_vectors(vector<int> a, vector<int> b, size_t n){
            size_t i;
            vector<int> ret(n, 0);
            for(i = 0; i < n; ++i) {
                ret[i] = a[i] + b[i];
            }
            return ret;
        }

    public:

        // Constructor
        Measurement_model(int target_patch_height, int target_patch_width){
            patch_height = height_conversion(target_patch_height);
            cout << "new_height " << patch_height << '\n';
            patch_width = width_conversion(target_patch_width);
            cout << "patch_width " << patch_width << '\n';

            num_rows = MAP_HEIGHT/patch_height;
            num_cols = MAP_WIDTH/patch_width;
        }

        vector<int>** matrix_from_file(string map_filename){
            matrix = CreateVectorMatrix<int>(num_rows, num_cols, N_CLASSES);
            cout << "matrix[0][0][0]: " << matrix[0][0][0] << '\n';
            //
            //read files here and getting the center coordinates x & y
            int x1, x2, y1, y2;
            int class_id = 0;

            fstream newfile;
            newfile.open(map_filename.c_str(),ios::in); //open a file to perform read operation using file object
            if (newfile.is_open())
            {
                string tp;
                char mot[10];
                while(getline(newfile, tp)) //EVERY LINE
                {
                    int i = 0;
                    int m = 0;
                    cout << "line: " << tp << '\n';

                    for (string::iterator it=tp.begin(); it!=tp.end(); ++it)
                    {
                        if(*it != ' ')
                        {
                            mot[i] = *it;
                            i++;
                        }

                        else
                        {
                            mot[i] = '\0';
                            std::istringstream iss (mot);
                            if(m == 0)
                            {
                                sscanf(mot, "%d", &x1);
                                cout << "x1: "<<x1<<"--";
                            }
                            if(m == 1)
                            {
                                sscanf(mot, "%d", &y1);
                                cout << "y1: "<<y1<<"--";
                            }
                            if(m == 2)
                            {
                                sscanf(mot, "%d", &x2);
                                cout << "x2: "<<x2<<"--";
                            }
                            if(m == 3)
                            {
                                sscanf(mot, "%d", &y2);
                                cout << "y2: "<<y2<<"--";
                            }
                            if(m == 4)
                            {
                                sscanf(mot, "%d", &class_id);
                                cout << "id: "<<class_id<<'\n'<<'-'<<'\n'<<'-'<<'\n';
                            }
                            i = 0;
                            m++;
                        }
                    }
                    int x = (x1 + x2) / 2;
                    int y = (y1 + y2) / 2;
                    int new_x = floor(x/patch_height);
                    int new_y = floor(y/patch_width);
                    cout << new_x <<':'<< new_y<<'\n';
                    matrix[new_x][new_y][class_id] += 1;
                    cout << "matrix[" << new_x << "][" << new_y << "][" << class_id << "] = " << matrix[new_x][new_y][class_id] << '\n';
                }
                newfile.close(); //close the file object.
            }
            cout << "matrix[0][0].size(): " << matrix[0][0].size()<< '\n';
            int sum = 0;
            for( int i = 0; i < num_rows; i++){
                for( int j = 0; j < num_cols; j++){
                    if(matrix[i][j].size() == 60){
                        sum += 1;
                    }
                }
            }
            cout << "num_rows*num_cols = " << num_rows*num_cols << " sum: " << sum << '\n';
        }

        /**
         * Computes a sum of all the patches included
         * in the virtual view of the particle.
         * Each element of the matrix is a patch,
         * which contains only one value.
         */
        vector<int> sum_from_cross_simple(vector<double> pose){
            int rowcoord = pose[0]/patch_width; // The row location of particle
            int colcoord = pose[1]/patch_height; // The column location of particle

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

            int num_patches_w = avb_w_px/patch_width;
            int num_patches_h = avb_h_px/patch_height;

            // TODO: Handle case when particle is near border of map...
            // Get values from matrix, around particle coordinates
            vector<int> sum(N_CLASSES, 0);
            int start_w = rowcoord - num_patches_w/2; // TODO? Handle float sitations?
            for(int i = start_w; i < num_patches_w; i++){
                sum = add_int_vectors(sum, matrix[i][colcoord], N_CLASSES);
            }
            int start_h = colcoord - num_patches_h/2;
            for(int i = start_h; i < num_patches_h; i++){
                sum = add_int_vectors(sum, matrix[rowcoord][i], N_CLASSES);
            }

            return sum;
        }
};

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

// Rotating CV Points, used for plotting.
Point rotate(Point pt, Point orig, double theta){
    double tx,ty;
    tx = pt.x-orig.x;
    ty = pt.y-orig.y;
    pt.x = tx*cos(theta) - ty*sin(theta) + orig.x;
    pt.y = ty*cos(theta) + tx*sin(theta) + orig.y;

    return pt;
}

int main(){



    vector<double> pose(4,0);
    pose[0] = 500;
    pose[1] = 500;
    pose[2] = 20;
    pose[3] = 90;


    Measurement_model mm = Measurement_model(2000,2000);
    vector<int> a(4, 3);
    vector<int> b(4, 4);
    vector<int> sum = mm.add_int_vectors(a,b, 4);
    for(int i = 0; i < sum.size(); i++){
        printf("%d, ", sum[i]);

    }
    mm.matrix_from_file("test.txt");
    sum = mm.sum_from_cross_simple(pose);
    for(int i = 0; i < sum.size(); i++){
        printf("%d, ", sum[i]);

    }


    /*
    imshow("title", cnt_img);
    waitKey(0);
    destroyWindow("title");

    Point pt;
    pt.x = 100;
    pt.y = 100;
    circle(cnt_img, pt, 3, Scalar(0, 0, 255), FILLED, LINE_AA);
    imshow("title", cnt_img);
    waitKey(0);
    destroyWindow("title");

    line(cnt_img, pt, Point(pt.x-10,pt.y-10), Scalar(0,255,255), 5, 8, 0);
    imshow("title", cnt_img);
    waitKey(0);
    destroyWindow("title");

    rectangle( cnt_img, Point(w*0.33, w*0.33), Point(w*0.66, w*0.66),Scalar(0,255,255), 5, 8, 0);
    imshow("title", cnt_img);
    waitKey(0);
    destroyWindow("title");
    */
    /*

    namedWindow("title");
    int w = 1000;
    int h = 1000;
    Mat cnt_img = Mat::zeros(w, w, CV_8UC3);
    // Plot Grid
    int rows = 50;
    int cols = 50;
    for(int i = 0; i < rows-1; i++){
        for(int j = 0; j < cols-1; j++){
            int x = w*i/rows;
            int y = h*j/cols;
            int x2 = w*(i+1)/rows;
            int y2 = h*(j+1)/cols;
            rectangle(cnt_img, Point(x,y), Point(x2,y2), Scalar(90,90,90), 1, 8, 0);
        }
    }

    // Plot Particle
    vector<double> pose(4,0);
    pose[0] = 500;
    pose[1] = 500;
    pose[2] = 20;
    pose[3] = 90;

    Point orig = Point(pose[0], pose[1]);

    int height = pose[2]*ARHEIGHT/MPPX;
    int width = pose[2]*ARWIDTH/MPPX;

    vector<Point> pts(4);
    pts[0] = Point(pose[0]-width/2, pose[1]+height/2);
    pts[1] = Point(pose[0]+width/2, pose[1]+height/2);
    pts[2] = Point(pose[0]+width/2, pose[1]-height/2);
    pts[3] = Point(pose[0]-width/2, pose[1]-height/2);

    double theta = pose[3]*CV_PI/180;

    circle(cnt_img, orig, 3, Scalar(0,0,255), FILLED, LINE_AA);


    for(int haha=0; haha<360; haha++){

        cnt_img = Scalar(255, 255, 255);

        double deg = haha*CV_PI/180;

        for(int i = 0; i < pts.size(); i++){
            circle(cnt_img, pts[i], 3, Scalar(255, 0, 0), FILLED, LINE_AA);
            line(cnt_img, pts[i], pts[(i+1)%pts.size()], Scalar(255,0,0), 1, 8, 0);
        }


        for(int i = 0; i < pts.size(); i++){
            Point pt = rotate(pts[i], orig, deg);
            circle(cnt_img, pt, 3, Scalar(0, 0, 255), FILLED, LINE_AA);

            Point pt1 = rotate(pts[i], orig, deg);
            Point pt2 = rotate(pts[(i+1)%pts.size()], orig, deg);
            line(cnt_img, pt1, pt2, Scalar(0,0,255), 1, 8, 0);
        }

        imshow("title", cnt_img);
        waitKey(11);
    }
    destroyWindow("title");



    //printf("x0: %d, y0: %d, x0n: %d, y0n: %d \n",x0,y0,x0n,y0n);
    */
    /*
    imshow("title", cnt_img);
    waitKey(0);
    destroyWindow("title");
    */

    /*
    Point pt = Point(pose[0],pose[1]);
    circle(cnt_img, pt, 3, Scalar(0, 0, 255), FILLED, LINE_AA);
    line(cnt_img, pt, Point(pt.x-10,pt.y-10), Scalar(0,255,255), 1, 8, 0);
    imshow("title", cnt_img);
    waitKey(0);
    destroyWindow("title");
    */


    return 0;
}

