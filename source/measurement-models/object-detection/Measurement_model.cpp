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
        void pxcoords_mcoords(int pxx, int pxy, int &mx, int &my){
            mx = pxx/patch_width;
            my = pxy/patch_height;
        }
        //
        // Rotating CV Points, used for plotting.
        Point rotate(Point pt, Point orig, double theta){
            double tx,ty;
            tx = pt.x-orig.x;
            ty = pt.y-orig.y;
            pt.x = tx*cos(theta) - ty*sin(theta) + orig.x;
            pt.y = ty*cos(theta) + tx*sin(theta) + orig.y;

            return pt;
        }


        vector<Point> create_box(vector<double> pose){
            //int altitude = convert_to_altitude(pose[2]);
            int altitude = 90; //meters
            int width = pose[2]*ARWIDTH/MPPX;// pixels
            int height = pose[2]*ARHEIGHT/MPPX;// pixels

            Point center = Point(pose[0],pose[1]);
            // Create viewbox
            vector<Point> pts(4);
            pts[0] = Point(pose[0]-width/2, pose[1]+height/2); // bottom left
            pts[1] = Point(pose[0]+width/2, pose[1]+height/2); // bottom right
            pts[2] = Point(pose[0]+width/2, pose[1]-height/2); // top right
            pts[3] = Point(pose[0]-width/2, pose[1]-height/2); // top left

            double theta = pose[3]*CV_PI/180; // assuming pose[3] is in deg

            // Rotate box to align with particle orientation
            vector<Point> a_pts(4);
            for(int i = 0; i < 4; i++){
                a_pts[i] = rotate(pts[i], center, theta);
            }

            return a_pts;
        }

        /**
         * This currently only works for angles between 0 and 90
        */
        Point create_step(Point a, Point b){
            Point step;
            step.x = abs(b.x - a.x);
            step.x = min(step.x, patch_width);
            step.y = abs(b.y - a.y);
            step.y = min(step.y, patch_width);
            /*
            double rise;
            if(step.x != 0){
                rise = abs(double(b.y-a.y)/double(b.x-a.x));
                step.y = rise*step.x;
            }else{
                step.y = patch_height;
            }
            */
            return step;
        }

        Point create_step(Point a, Point b, double theta){
            Point step;


            return step;
        }


        void mcoords_pxcoords(int mx, int my, int &pxx, int &pxy){
            pxx = mx*patch_width;
            pxy = my*patch_height;
        }
        /**
         * Returns a vector if coordinates
        */
        void viewbox_content(Mat &img, vector<Point> pts){
            Point left(MAP_WIDTH,0);
            Point right(0,0);
            Point top(0,MAP_HEIGHT); //initialize with the opposite of goal
            Point bottom(0,0);
            for(int i = 0; i < pts.size(); i++){
                if(left.x > pts[i].x){
                    left = pts[i];
                }
                if(right.x < pts[i].x){
                    right = pts[i];
                }
                if(top.y > pts[i].y){
                    top = pts[i];
                }
                if(bottom.y < pts[i].y){
                    bottom = pts[i];
                }
            }
            circle(img, left, 50, Scalar(0,0,255), FILLED, LINE_AA);
            circle(img, right, 50, Scalar(0,0,255), FILLED, LINE_AA);
            circle(img, top, 50, Scalar(0,0,255), FILLED, LINE_AA);
            circle(img, bottom, 50, Scalar(0,0,255), FILLED, LINE_AA);
            cout << "aftr left right top bottom" << '\n';
            // Matrix coordinates for corner points
            int mx_l, my_l, mx_r, my_r, mx_t, my_t, mx_b, my_b;
            pxcoords_mcoords(left.x, left.y, mx_l, my_l);
            pxcoords_mcoords(right.x, right.y, mx_r, my_r);
            pxcoords_mcoords(top.x, top.y, mx_t, my_t);
            pxcoords_mcoords(bottom.x, bottom.y, mx_b, my_b);


            Point mleft(mx_l,my_l);
            Point mright(mx_r,my_r);
            Point mtop(mx_t,my_t); //initialize with the opposite of goal
            Point mbottom(mx_b,my_b);
            circle(img, mleft, 50, Scalar(0,255,0), FILLED, LINE_AA);
            circle(img, mright, 50, Scalar(0,255,0), FILLED, LINE_AA);
            circle(img, mtop, 50, Scalar(0,255,0), FILLED, LINE_AA);
            circle(img, mbottom, 50, Scalar(0,255,0), FILLED, LINE_AA);

            cout << "aftr mcoords" << '\n';
            cout << "left.x " << left.x << " left.y " << left.y << '\n';
            cout << "right.x " << right.x << " right.y " << right.y << '\n';
            cout << "top.x " << top.x << " top.y " << top.y << '\n';
            cout << "bottom.x " <<  bottom.x << " bottom.y " << bottom.y << '\n';

            // Create vectors to store the max and min rowidx for each colidx
            int tmp = mx_r - mx_l;
            cout << "mx_r" << mx_r << "mx_l" << mx_l << '\n';

            vector<int> maxv(tmp,0);
            cout << "aftr max" << '\n';

            vector<int> minv(tmp,MAP_HEIGHT);
            Point step, idx;

            int mx, my;
            cout << "before create step" << '\n';
            step = create_step(left, top);
            cout << "aftr create step" << '\n';
            idx = left;
            while(idx.y > top.y && idx.x < right.x){
                pxcoords_mcoords(idx.x, idx.y, mx, my);
                // top and bottom are inverse. top is lowest..
                cout << mx - mx_l << '\n';
                if(my > maxv[mx-mx_l]){
                    maxv[mx-mx_l] = my;
                }
                if(my < minv[mx-mx_l]){
                    minv[mx-mx_l] = my;
                }
                idx.x += step.x;
                idx.y -= step.y;
            }
            cout << "aftr left top" << '\n';
            step = create_step(left, bottom);
            idx = left;
            while(idx.y < bottom.y && idx.x < right.x){
                pxcoords_mcoords(idx.x, idx.y, mx, my);
                // top and bottom are inverse. top is lowest..
                if(my > maxv[mx-mx_l]){
                    maxv[mx-mx_l] = my;
                }
                if(my < minv[mx-mx_l]){
                    minv[mx-mx_l] = my;
                }
                idx.x += step.x;
                idx.y += step.y;
            }
            cout << "aftr left bottom" << '\n';
            step = create_step(top, right);
            idx = top;
            while(idx.y < bottom.y && idx.x < right.x){
                pxcoords_mcoords(idx.x, idx.y, mx, my);
                // top and bottom are inverse. top is lowest..
                cout << "mx " << mx << " mx_l " << mx_l << '\n';
                if(my > maxv[mx-mx_l-1]){
                    maxv[mx-mx_l-1] = my;
                }
                if(my < minv[mx-mx_l-1]){
                    minv[mx-mx_l-1] = my;
                }
                idx.x += step.x;
                idx.y += step.y;
            }
            cout << "aftr top right" << '\n';
            step = create_step(bottom, right);
            idx = bottom;
            while(idx.y > top.y && idx.x < right.x){
                pxcoords_mcoords(idx.x, idx.y, mx, my);
                // top and bottom are inverse. top is lowest..
                if(my > maxv[mx-mx_l-1]){
                    maxv[mx-mx_l-1] = my;
                }
                if(my < minv[mx-mx_l-1]){
                    minv[mx-mx_l-1] = my;
                }
                idx.x += step.x;
                idx.y -= step.y;
            }
            cout << "aftr bottom right" << '\n';
            for(int i = 0; i < maxv.size(); i++){
                cout << "maxv[" << i << "]: " << maxv[i] << '\n';
                Point pt = Point((i+mx_l)*patch_width, maxv[i]*patch_height);
                //circle(img, pt, 50, Scalar(0,0,255), FILLED, LINE_AA);
                cout << "minv[" << i << "]: " << minv[i] << '\n';
                Point pt2 = Point((i+mx_l)*patch_width, minv[i]*patch_height);
                //circle(img, pt2, 50, Scalar(0,0,255), FILLED, LINE_AA);
            }

        }


        vector<int> add_int_vectors(vector<int> a, vector<int> b, size_t n){
            size_t i;
            vector<int> ret(n, 0);
            for(i = 0; i < n; ++i) {
                ret[i] = a[i] + b[i];
            }
            return ret;
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
                    matrix[new_y][new_x][class_id] += 1;
                    cout << "matrix[" << new_y << "][" << new_x << "][" << class_id << "] = " << matrix[new_y][new_x][class_id] << '\n';
                    cout << "matrix[0][0][48] " << matrix[0][0][48] << '\n';
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
                    cout << "matrix[0][0][48] " << matrix[0][0][48] << '\n';
        }

        /**
         * Computes a sum of all the patches included
         * in the virtual view of the particle.
         * Each element of the matrix is a patch,
         * which contains only one value.
         */
        vector<int> sum_from_cross_simple(vector<double> pose){
                    cout << "matrix[0][0][48] " << matrix[0][0][48] << '\n';
            int rowcoord = pose[0]/patch_width; // The row location of particle
            int colcoord = pose[1]/patch_height; // The column location of particle
            printf("rowcoord %d, colcoord %d \n", rowcoord, colcoord);

            // Find width and height of viewbox, in pixels.
            //int altitude = convert_to_altitude(pose[2]);
            int altitude = 93;
            int vb_w_px = altitude*ARWIDTH/MPPX;
            int vb_h_px = altitude*ARHEIGHT/MPPX;

            // Rotate viewbox to align with global frame, inside original boundaries (shrink it)
            // Assuming pose[3] is angle represented in degrees
            double rad = pose[3]*CV_PI/180;
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
            printf("avb_w_px %d, avb_h_px %d \n", avb_w_px, avb_h_px);

            int num_patches_w = avb_w_px/patch_width;
            int num_patches_h = avb_h_px/patch_height;

            // TODO: Handle case when particle is near border of map...
            // Get values from matrix, around particle coordinates
            vector<int> sum(N_CLASSES, 0);
            cout << "before sum of cross" << '\n';
            int start_w = rowcoord - num_patches_w/2; // TODO? Handle float sitation?
            cout << "start_w " << start_w << "start_w+num_patches_w" << start_w+num_patches_w << '\n';
            for(int i = start_w; i < start_w + num_patches_w; i++){
                cout << "i is: " << i << '\n';
                sum = add_int_vectors(sum, matrix[i][colcoord], N_CLASSES);
            }
            cout << "start_w " << start_w << " num_patches_h " << num_patches_h << '\n';
            int start_h = colcoord - num_patches_h/2;
            for(int i = start_h; i < start_h + num_patches_h; i++){
                cout << "i is: " << i << '\n';
                sum = add_int_vectors(sum, matrix[rowcoord][i], N_CLASSES);
            }

            return sum;
        }

        void plot_grid(Mat &img, string window_name){
            int w = MAP_WIDTH;
            int h = MAP_HEIGHT;
            img = Mat::zeros(w, w, CV_8UC3);
            img = Scalar(255,255,255);
            // Plot Grid
            for(int i = 0; i < num_rows; i++){
                for(int j = 0; j < num_cols; j++){
                    int x = w*i/num_rows;
                    int y = h*j/num_cols;
                    int x2 = x + patch_width;
                    int y2 = y + patch_height;
                    rectangle(img, Point(x,y), Point(x2,y2), Scalar(90,90,90), 3, 8, 0);
                }
            }
        }

        void plot_particle(Mat &img, vector<double> pose){
            Point pt =  Point(pose[0], pose[1]);
            Point pt2 = Point(pt.x, pt.y-100);
            pt2 = rotate(pt2, pt, pose[3]*CV_PI/180);

            circle(img, pt, 20, Scalar(0,0,255), FILLED, LINE_AA);
            line(img, pt, pt2, Scalar(0,255,0), 12, 8, 0);

        }

        void plot_viewbox(Mat &img, vector<Point> pts){
            circle(img, pts[0], 15, Scalar(0,0,255), FILLED, LINE_AA);
            for(int i = 0; i < pts.size(); i++){
                circle(img, pts[i], 3, Scalar(255, 0, 0), FILLED, LINE_AA);
                line(img, pts[i], pts[(i+1)%pts.size()], Scalar(255,0,0), 8, 8, 0);
            }
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


int main(){
    vector<double> pose(4,0);

    Measurement_model mm = Measurement_model(240,240);
    vector<int> a(4, 3);
    vector<int> b(4, 4);
    vector<int> sum = mm.add_int_vectors(a,b, 4);
    printf("SUM: \n");
    for(int i = 0; i < sum.size(); i++){
        printf("%d, ", sum[i]);

    }
    mm.matrix_from_file("test.txt");

    string window_name = "map";
    Mat img;
    mm.plot_grid(img,window_name);

    pose[0] = 2400;
    pose[1] = 2400;
    pose[2] = 150;
    pose[3] = 45;
    mm.plot_particle(img, pose);

    vector<Point> a_pts = mm.create_box(pose);
    mm.plot_viewbox(img, a_pts);
    //mm.viewbox_content(img, a_pts);

    pose[0] = 1000;
    pose[1] = 1000;
    pose[2] = 200;
    pose[3] = 0;
    mm.plot_particle(img, pose);

    vector<Point> a_pts2 = mm.create_box(pose);

    pose[0] = 3000;
    pose[1] = 3500;
    pose[2] = 90;
    pose[3] = 270;
    mm.plot_particle(img, pose);

    vector<Point> a_pts3 = mm.create_box(pose);
    mm.plot_viewbox(img, a_pts3);   mm.plot_viewbox(img, a_pts2);
    //mm.viewbox_content(img, a_pts);

    //Point step = mm.create_step(a_pts[0], a_pts[3]);

    namedWindow(window_name, WINDOW_NORMAL);
    imshow(window_name, img);
    waitKey(0);
    destroyWindow(window_name);

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

