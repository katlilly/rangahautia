#ifndef VBYTE_COMPRESSOR_H_
#define VBYTE_COMPRESSOR_H_

#include <cstdint>
class VBcompressor
{
public:
  uint32_t *input;
  uint8_t *output;
  int input_length;
  int output_length;
  uint32_t current_in;
  uint8_t *current_out;
  int current_out_length;
    
public:
  
  VBcompressor(uint32_t *raw, int length)
  {
    input = raw;
    input_length = length;
    output = new uint8_t [5*length];
    output_length = 0;
    current_out = new uint8_t [5];
  }

  ~VBcompressor()
    {
      delete [] output;
      delete [] current_out;
    }
    
 public:
  void vb_compress_int(uint32_t number);
  void compress_array();
};

#endif
