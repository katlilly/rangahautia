#include <stdio.h>
#include <stdlib.h>
//#include <cstdint>
#include <stdint.h>
#include "vbyte_decompress.h"

void VBdecompressor::decompress_array()
{
  uint32_t decompressed = 0;
  int compressedindex = 0;
  int decompressedindex = 0;
  int shift = 0;
  uint8_t mask = 127;
  uint8_t continuebit = 128;
  
  while (compressedindex < input_length)
    {
      uint8_t current = input[compressedindex++];
      uint8_t bits = current & mask;
      decompressed = decompressed | (bits << (7*shift));
      shift++;
	
      if ((continuebit & current) == 0)
	{
	  shift = 0;
	  output_length++;
	  output[decompressedindex++] = decompressed;
	  decompressed = 0;
	}
    }
}


// int main(void)
// {
//   int testlength = 4;
//   uint8_t *original = (uint8_t *) malloc(testlength * sizeof(*original));
//   original[0] = 1;
//   original[1] = 2;
//   original[2] = 129;
//   original[3] = 2;
//   original[4] = 145;
//   original[5] = 143;
//   original[6] = 1;

//   VBdecompressor *vbdc = new VBdecompressor(original, testlength);
//   vbdc->decompress_array();
  
//   uint32_t *compressed = vbdc->output;
//   int decompressed_length = vbdc->output_length;
//   for (int i = 0; i < vbdc->output_length; i++)
//     {
//       printf("%d\n", vbdc->output[i]);
//     }
  

//   return 0;
// }


