#ifndef VBYTE_DECOMPRESS_H_
#define VBYTE_DECOMPRESS_H_
#include <stdint.h>

class VBdecompressor
{
public:
  uint8_t *input;
  uint32_t *output;
  int input_length;
  int output_length;
  uint8_t *current_in;
  uint32_t current_out;
  int current_out_length;

 public:
  VBdecompressor(uint8_t *compressed, int length)
    {
      input = compressed;
      input_length = length;
      output = new uint32_t [length];
      output_length = 0;
      current_out = 0;
    }

  ~VBdecompressor()
    {
      delete [] output;
    }
    
 public:
  
  void decompress_array();
  
};

#endif
