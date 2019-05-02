#include <stdio.h>
#include <stdlib.h>
#include <cstdint>
#include "vbyte_compress.h"


int VBcompress::decompress(uint32_t *decompressed, uint8_t *compressed, int compressed_length)
{
  uint8_t mask = 127;
  uint8_t continuebit = 128;
  int compressed_index = 0;
  int decompressed_index = 0;
  int shift = 0;
  uint32_t result = 0;
  uint8_t current, bits;
  
  while (compressed_index < compressed_length)
    {
      current = compressed[compressed_index++];
      bits = current & mask;
      result = result | (bits << (7*shift++));

      if ((continuebit * current) == 0)
	{
	  shift = 0;
	  printf("%d\n", result);
	  decompressed[decompressed_index++] = result;
	  result = 0;
	}
    }
  return decompressed_index;
}


int VBcompress::compress(uint8_t *compressed, uint32_t *raw, int raw_length)
{
  uint8_t *current = new uint8_t [5];
  int compressed_length = 0;
  uint8_t mask = 127;
  uint8_t continuebit = 128;
  uint8_t result = 0;;
  
  for (int i = 0; i < raw_length; i++)
    {
      uint32_t num_to_compress = raw[i];
      
      if (num_to_compress < 1)
	{
	  printf("don't send numbers less than 1 to this function\n");
	  return 0;
	}
      
      //compress current int into current 8bit array
      int index = 0;
      while (num_to_compress > 0)
	{
	  result = num_to_compress & mask;
	  num_to_compress = num_to_compress >> 7;
	  if (num_to_compress > 0)
	    {
	      //printf("this happened\n");
	      result = result | continuebit;
	    }
	  current[index++] = result;
	}
      // copy from current to compressed
      for (int i = 0; i < index; i++)
	{
	  compressed[compressed_length + i] = current[i];
	}
      compressed_length += index;
       
    }
  return compressed_length;
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
  
  printf("original: ");
  for (int i = 0; i < testlength; i++)
    printf("%d, ", original[i]);
  printf("\n");

  VBcompress *compressor = new VBcompress();
  uint8_t *encoded = new uint8_t [5 * testlength];
  int comp_len = compressor->compress(encoded, original, testlength);

  printf("compressed: ");
  for (int i = 0; i < comp_len; i++)
    printf("%d, ", encoded[i]);
  printf("\n");

  uint32_t *decoded = new uint32_t [comp_len];
  int length = compressor->decompress(decoded, encoded, comp_len);

  printf("decompressed: ");
  for (int i = 0; i < length; i++)
    printf("%d, ", decoded[i]);
  printf("\n");
  
  
  return 0;
}
