#ifndef GROWABLEARRAY_H_
#define GROWABLEARRAY_H_

class Growablearray
{
 public:
  int capacity;
  int itemcount;
  int *items;

 public:
  Growablearray()
    {
      capacity = 2;
      itemcount = 0;
      items = (int *) malloc(2 * sizeof(*items));
      items[0] = 0;
      items[1] = 0;
    }

 public:
  void append(int item);
  void print();
};

#endif
