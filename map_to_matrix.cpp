#include <iostream>
#include <cstring>
#include <stdio.h>
#include <math.h>
using namespace std;

#define MAP_WIDTH 4800
#define MAP_HEIGHT 4800

typedef struct _patch{
  int instances;
  string object;
  struct _patch* suiv = NULL;
} patch;

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

patch** tab = new patch*[target_height];
for( int i = 0; i < target_height; i++ ) {
  patch* row = new patch[target_width];
  tab[i] = row;
}


//read files here and getting the center coordinates x & y

int x = 0;
int y = 0;

new_x = ;
new_y = ;

return 0;
}
