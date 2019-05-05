#ifndef ATHTABLE_H_
#define ATHTABLE_H_

class Htable
{
 public:
  
  int table_size;
  
   struct entry {
    char *key;
    void *value;
  } ;


  //private:
  entry *table;

 public:
  virtual void callback(char *key, void *value)
  {
    // nothing
  }
  
  bool add(char *key, void *value);
  void *find(char *key);
  void print();
  void write(char *key, void *value);
  void iterate();
  
  Htable(u_int32_t size)
    {
      table_size = size;
      //entry table[size];
      table = (entry *) malloc(sizeof(*table) * size);
      for (u_int32_t i = 0; i < size; i++)
	{
	  table[i].key = NULL;
	  table[i].value = NULL;
	}
    }

  ~Htable()
    {
      //delete [] table;
      for (int i = 0; i < table_size; i++)
      	{
      	  free(table[i].key);
      	  free(table[i].value);
      	}
      free(table);
    }
  
  //u_int32_t hash_string(char *key);

 private:
  void iterate_callback(char *key, void *value);
  u_int32_t hash_string(char *key);
  //void iterate(iterate_callback callback);

};

#endif
