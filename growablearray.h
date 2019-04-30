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
      items = new int [2];
      items[0] = 0;
      items[1] = 0;
    }
  
  Growablearray(int docno)
    {
      capacity = 2;
      itemcount = 2;
      items = new int [2]; 
      items[0] = docno;
      items[1] = 1;
    }
  
  ~Growablearray()
    {
      delete [] items;
    }
  
 public:
  void append(int item);
  void print();
};

#endif
