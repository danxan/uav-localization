#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <math.h>
#include <sstream>
#include <vector>

#include "tmp.h"

using namespace std;

#define MAP_WIDTH 4800
#define MAP_HEIGHT 4800
#define N_CLASSES 60

/*
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
*/

int width_conversion(int batch_width)  //batch size is in pixels
{
    int i = batch_width;
    while((i != MAP_WIDTH) && (MAP_WIDTH % i != 0))
    {
        i++;
    }
    int j = batch_width;
    while((j != 0) && (MAP_WIDTH % j != 0))
    {
        j--;
    }

    if(batch_width - j < i - batch_width)
    {
        return j;
    }
    else
        return i;
}

int height_conversion(int batch_height)  //batch size is in pixels
{
    int i = batch_height;
    while((i != MAP_HEIGHT) && (MAP_HEIGHT % i != 0))
    {
        i++;
    }
    int j = batch_height;
    while((j != 0) && (MAP_HEIGHT % j != 0))
    {
        j--;
    }

    if(batch_height - j < i - batch_height)
    {
        return j;
    }
    else
        return i;
}

vector<int>** matrix_from_file(int target_batch_height, int target_batch_width, string map_filename){
    int batch_height = height_conversion(target_batch_height);
    cout << "new_height " << batch_height << '\n';
    int batch_width = width_conversion(target_batch_width);
    cout << "batch_width " << batch_width << '\n';

    int num_rows = MAP_HEIGHT/batch_height;
    int num_cols = MAP_WIDTH/batch_width;

    vector<int>** matrix = CreateVectorMatrix<int>(num_rows, num_cols, N_CLASSES);
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
            int new_x = floor(x/batch_height);
            int new_y = floor(y/batch_width);
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


    return matrix;
}

int main(){
    int target_height = 31;
    int target_width = 13;

/*
 *
    int new_height = height_conversion(target_height);
    cout << "new_height " << new_height << '\n';
    int new_width = width_conversion(target_width);
    cout << "new_width " << new_width << '\n';

    //read files here and getting the center coordinates x & y
    int x1, x2, y1, y2;
    int class_id = 0;

    fstream newfile;
    newfile.open("test.txt",ios::in); //open a file to perform read operation using file object
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
            int new_x = floor(x/new_height);
            int new_y = floor(y/new_width);
            cout << new_x <<':'<< new_y<<'\n';
        }
        newfile.close(); //close the file object.
    }

*/
    matrix_from_file(target_height, target_width, "test.txt");

    return 0;
}
