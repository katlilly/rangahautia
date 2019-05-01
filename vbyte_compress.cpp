#include <stdio.h>
#include <stdlib.h>
#include <cstdint>
#include "vbyte_compress.h"


void VBcompressor::vb_compress_int(uint32_t number)
{
  int index = 0;
  if (number < 1)
    {
      printf("don't send numbers less than 1 to this function\n");
      return;
    }
  while (number > 0)
    {
      uint8_t mask = 127;
      uint8_t continuebit = 128;
      uint8_t result = number & mask;
      number = number >> 7;
      if (number > 0)
	{
	  result = result | continuebit;
	}
      current_out[index++] = result;
    }
  current_out_length = index;
}


void VBcompressor::compress_array()
{
  for (int i = 0; i < input_length; i++)
    {
      vb_compress_int(input[i]);
      for (int j = 0; j < current_out_length; j++)
	{
	  output[output_length+j] = current_out[j];
	}
      output_length += current_out_length;
    }
}
  
   			 



/* 
   18321 should compress to 145, 143, 1
*/

int main(void)
{
  int testlength = 4;
  uint32_t *original = (uint32_t *) malloc(testlength * sizeof(*original));
  original[0] = 1;
  original[1] = 2;
  original[2] = 257;
  original[3] = 18321;

  VBcompressor *vbc = new VBcompressor(original, testlength);
  vbc->compress_array();
  
  uint8_t *compressed = vbc->output;
  int compressed_length = vbc->output_length;
  for (int i = 0; i < vbc->output_length; i++)
    {
      printf("%d\n", vbc->output[i]);
    }
  
  
  return 0;
}
