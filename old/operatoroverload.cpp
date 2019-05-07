#include <stdio.h>
#include "array.h"

int main(void)
{
  int [] array = {1, 2, 3, 4, 5};
  Array *x = new Array(array, 5);
  x->print();
  x[2] = 47;
  x->print();

  return 0;
}
