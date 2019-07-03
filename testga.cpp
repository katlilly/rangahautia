#include <stdio.h>
#include <cstdint>
#include "growablearray.h"

int main(void)
{
  Growablearray test;
  test.append(1);
  test.append_two(2, 3);
  test.print();
  uint32_t *array = test.to_uint32_array();
  test.print();

  return 0;
}
