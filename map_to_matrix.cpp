#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <math.h>
#include <sstream>
using namespace std;

#define MAP_WIDTH 4800
#define MAP_HEIGHT 4800
#define N_CLASSES 60

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

int main(){

int target_height = 31;
int target_width = 13;

int new_height = height_conversion(target_height);
int new_width = width_conversion(target_width);

int*** tab = new int**[target_height];
for( int i = 0; i < target_height; i++ )
{
  int** row = new int*[target_width];
  for( int j = 0; j < N_CLASSES; j++ )
  {
    int* patch = new int[N_CLASSES];
    row[j] = patch;
  }
  tab[i] = row;
}


//read files here and getting the center coordinates x & y
int x1, x2, y1, y2;
int class_id = 0;

fstream newfile;
newfile.open("test.txt",ios::in); //open a file to perform read operation using file object
if (newfile.is_open())
{
  string tp;
  char* mot;
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

return 0;
}
