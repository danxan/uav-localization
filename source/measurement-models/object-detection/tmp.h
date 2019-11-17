#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <math.h>
#include <sstream>
#include <vector>
#include <bits/stdc++.h>

using namespace std;


#define MPPX 0.243 // dependent on spatial resolution of sattelite image
#define ARWIDTH 1.87 // 1.87 is dependent on aspect ratio
#define ARHEIGHT 1.05 // 1.05 is dependent on aspect ratio

#define PI 3.14159265

#define MAP_WIDTH 4800
#define MAP_HEIGHT 4800
#define N_CLASSES 60
//
// Thanks to v.chaplin on stackoverflow,
// under the thread:
// "C++ 3d array - dynamic memory allocation aligned in one line"
// The code is modified to our liking
template <class T> T ***Create3D(int N1, int N2, int N3)
{
    T *** array = new T ** [N1];

    array[0] = new T * [N1*N2];

    array[0][0] = new T [N1*N2*N3];

    int i,j,k;

    for( i = 0; i < N1; i++) {

        if (i < N1 -1 ) {

            array[0][(i+1)*N2] = &(array[0][0][(i+1)*N3*N2]);

            array[i+1] = &(array[0][(i+1)*N2]);

        }

        for( j = 0; j < N2; j++) {
            if (j > 0) array[i][j] = array[i][j-1] + N3;
        }

    }

    cout << endl;
    return array;
};

template <class T> void Delete3D(T ***array) {
    delete[] array[0][0];
    delete[] array[0];
    delete[] array;
};

template <class T> vector<T> **CreateVectorMatrix(int N1, int N2, int N3)
{
    vector<T> ** array = new vector<T> * [N1];

    array[0] = new vector<T>  [N1*N2];

    int i,j;

    for( i = 0; i < N1; i++) {

        if (i < N1 -1 ) {

            array[i+1] = &(array[0][(i+1)*N2]);
        }
    }

    for( j = 0; j < N1*N2; j++){
        array[0][j] = vector<T>(N3, 0);

    }

    cout << endl;
    return array;
};

template <class T> void DeleteVectorMatrix(T ***array) {
    delete[] array[0];
    delete[] array;
};


int width_conversion(int batch_width);
int height_conversion(int batch_height);
vector<int>** matrix_from_file(int target_batch_height, int target_batch_width, string map_filename);
int compare_lists(vector<int> uav, vector<int> particle, vector<string> dict, int num_classes);
