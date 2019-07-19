#pragma once
#include <cstdint>
#include <immintrin.h>

class Simple10avx
{
 public:
  struct row {
    int intsper32;
    int bitwidth;
  };

  row *table;
  int num_compressed_512bit_words = 0;
  int num_compressed_32bit_words = 0;
  

 private:
  int num_selectors;
  
 public:
  Simple10avx()
    {
      num_selectors = 10;
      table = new row[num_selectors];
      
      table[0].intsper32 = 1;
      table[0].bitwidth = 32;
      
      table[1].intsper32 = 2;
      table[1].bitwidth = 16;

      table[2].intsper32 = 3;
      table[2].bitwidth = 10;

      table[3].intsper32 = 4;
      table[3].bitwidth = 8;

      table[4].intsper32 = 5;
      table[4].bitwidth = 6;

      table[5].intsper32 = 6;
      table[5].bitwidth = 5;

      table[6].intsper32 = 8;
      table[6].bitwidth = 4;

      table[7].intsper32 = 10;
      table[7].bitwidth = 3;

      table[8].intsper32 = 16;
      table[8].bitwidth = 2;

      table[9].intsper32 = 32;
      table[9].bitwidth = 1;
      
    }

  ~Simple10avx()
    {
      delete [] table;
    }

 public:
  void print_table();
  void print_512word_as_32ints(__m512i word);
  void dgaps_to_bitwidths(int *dest, int *source, int length);
  int encode(uint32_t *dest, uint32_t *raw, uint32_t *end, uint8_t *selectors);
  int decode(uint32_t *dest, uint32_t *encoded, uint32_t *end,
	     uint8_t *selectors, int num_sels);
 
 private:
  int encode_one_word(uint32_t *dest, uint32_t *raw, uint32_t* end, uint8_t *selector);
  int chose_selector(uint *raw, uint* end);
  int min(int a, int b);
  int decode_one_word(uint32_t *dest, uint32_t *encoded, uint32_t *end,
		      uint8_t *selectors);
};
