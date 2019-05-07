#include <stdio.h>
#include "array.h"


int Array::operator[](int index)
  {
    if (index >= size)
      {
	printf("out of bounds\n");
	return -1;
      }
    else
      {
	return items[index];
      }
  }


void Array::print() const
  {
    for (int i = 0; i < size; i++)
      printf("%d, ", items[i]);
    printf("\n");
  }
