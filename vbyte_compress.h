#ifndef VBYTE_COMPRESS_H_
#define VBYTE_COMPRESS_H_
#include <cstdint>

class VBcompress
{

 public:

  int compress(uint8_t *compressed, uint32_t *raw, int rawlength);
  int decompress(uint32_t *decompressed, uint8_t *compressed, int compressedlength);

  void unit_test(uint32_t *testdata, int testlength);

  ~VBcompress()
    {
      
    }
  
};

#endif
