#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BITMANIO_H_WHEREABOUTS "bitmanio.h"

#define BITMANIO_STORAGE_BITS 8
#include "bitmanio.h"
#define BITMANIO_STORAGE_BITS 16
#include "bitmanio.h"
#define BITMANIO_STORAGE_BITS 32
#include "bitmanio.h"
#define BITMANIO_STORAGE_BITS 64
#include "bitmanio.h"

#define TEST_SIZE             1024*256

//#define DEBUG

static int bits(uint64_t v) {
  int j = 63;
  while (j > 0 && (v & (1ULL << j)) == 0) {
    j--;
  }
  return j+1;
}

static int test_stream_random(void) {
  printf("%s\n", __FUNCTION__);
  uint8_t ref[TEST_SIZE];
  uint8_t mem8[TEST_SIZE];
  uint16_t mem16[TEST_SIZE];
  uint32_t mem32[TEST_SIZE];
  uint64_t mem64[TEST_SIZE];
  memset(mem8,0,sizeof(mem8));
  memset(mem16,0,sizeof(mem16));
  memset(mem32,0,sizeof(mem32));
  memset(mem64,0,sizeof(mem64));

  bitmanio_stream8_t bs8;
  bitmanio_stream16_t bs16;
  bitmanio_stream32_t bs32;
  bitmanio_stream64_t bs64;

  uint32_t i;
  for (i = 0; i < sizeof(ref); i++) {
    ref[i] = (rand() & 0xff) & rand();
  }

  bitmanio_init_stream8(&bs8, mem8);
  bitmanio_init_stream16(&bs16, mem16);
  bitmanio_init_stream32(&bs32, mem32);
  bitmanio_init_stream64(&bs64, mem64);

  for (i = 0; i < sizeof(ref); i++) {
    uint32_t v = ref[i];
    uint8_t b = bits(v);
    bitmanio_write8(&bs8, v, b);
    bitmanio_write16(&bs16, v, b);
    bitmanio_write32(&bs32, v, b);
    bitmanio_write64(&bs64, v, b);
  }

  bitmanio_init_stream8(&bs8, mem8);
  bitmanio_init_stream16(&bs16, mem16);
  bitmanio_init_stream32(&bs32, mem32);
  bitmanio_init_stream64(&bs64, mem64);

  for (i = 0; i < sizeof(ref); i++) {
    uint32_t v = ref[i];
    uint8_t b = bits(v);
    uint32_t v8 = bitmanio_read8(&bs8, b);
    uint32_t v16 = bitmanio_read16(&bs16, b);
    uint32_t v32 = bitmanio_read32(&bs32, b);
    uint32_t v64 = bitmanio_read64(&bs64, b);
    if (v8 != v) {
      printf("@%02x: differ %02x != exp %02x   8 %ib\n",i, v8, v, b);
      return -1;
    }
    if (v16 != v) {
      printf("@%02x: differ %02x != exp %02x  16 %ib\n",i, v16, v, b);
      return -1;
    }
    if (v32 != v) {
      printf("@%02x: differ %02x != exp %02x  32 %ib\n",i, v32, v, b);
      return -1;
    }
    if (v64 != v) {
      printf("@%02x: differ %02x != exp %02x  64 %ib\n",i, v64, v, b);
      return -1;
    }
  }

  return 0;
}

static int _test_stream(int zero) {
  uint64_t ref[TEST_SIZE];
  uint8_t mem8[TEST_SIZE];
  uint16_t mem16[TEST_SIZE];
  uint32_t mem32[TEST_SIZE];
  uint64_t mem64[TEST_SIZE];
  
  bitmanio_stream8_t bs8;
  bitmanio_stream16_t bs16;
  bitmanio_stream32_t bs32;
  bitmanio_stream64_t bs64;

  uint32_t i;
  for (i = 0; i < sizeof(ref)/sizeof(ref[0]); i++) {
    ref[i] = (((((((uint64_t)rand()) << 16) ^ (uint64_t)rand()) << 16) ^ (uint64_t)rand()) << 16) ^ (uint64_t)rand();
  }

  uint32_t j;
  for (j = 1; j <= 64; j++) {
    memset(mem8,zero ? 0xff : 0,sizeof(mem8));
    memset(mem16,zero ? 0xff : 0,sizeof(mem16));
    memset(mem32,zero ? 0xff : 0,sizeof(mem32));
    memset(mem64,zero ? 0xff : 0,sizeof(mem64));

    bitmanio_init_stream8(&bs8, mem8);
    bitmanio_init_stream16(&bs16, mem16);
    bitmanio_init_stream32(&bs32, mem32);
    bitmanio_init_stream64(&bs64, mem64);

    for (i = 0; i < sizeof(ref)/sizeof(ref[0]); i++) {
      uint64_t v = ref[i] & ((1ULL<<j)-1);
      if (zero) {
        if (j <= 8) bitmanio_writez8(&bs8, v, j);
        if (j <= 16) bitmanio_writez16(&bs16, v, j);
        if (j <= 32) bitmanio_writez32(&bs32, v, j);
        bitmanio_writez64(&bs64, v, j);
      } else {
        if (j <= 8) bitmanio_write8(&bs8, v, j);
        if (j <= 16) bitmanio_write16(&bs16, v, j);
        if (j <= 32) bitmanio_write32(&bs32, v, j);
        bitmanio_write64(&bs64, v, j);
      }
    }

    bitmanio_init_stream8(&bs8, mem8);
    bitmanio_init_stream16(&bs16, mem16);
    bitmanio_init_stream32(&bs32, mem32);
    bitmanio_init_stream64(&bs64, mem64);

    for (i = 0; i < sizeof(ref)/sizeof(ref[0]); i++) {
      uint64_t v = ref[i] & ((1ULL<<j)-1);
      uint32_t v8 = j <= 8 ? bitmanio_read8(&bs8, j) : 0;
      uint32_t v16 = j <= 16 ? bitmanio_read16(&bs16, j) : 0;
      uint32_t v32 = j <= 32 ? bitmanio_read32(&bs32, j) : 0;
      uint64_t v64 = bitmanio_read64(&bs64, j);
      if (j <= 8 && v8 != v) {
        printf("@%02x: differ %x != exp %lx   8 %ib\n",i, v8, v, j);
        return -1;
      }
      if (j <= 16 && v16 != v) {
        printf("@%02x: differ %x != exp %lx  16 %ib\n",i, v16, v, j);
        return -1;
      }
      if (j <= 32 && v32 != v) {
        printf("@%02x: differ %x != exp %lx  32 %ib\n",i, v32, v, j);
        return -1;
      }
      if (v64 != v) {
        printf("@%02x: differ %lx != exp %lx  64 %ib\n",i, v64, v, j);
        return -1;
      }
    }
  }

  return 0;
}

static int test_stream_nonzero(void) {
  printf("%s\n", __FUNCTION__);
  return _test_stream(0);
}

static int test_stream_zero(void) {
  printf("%s\n", __FUNCTION__);
  return _test_stream(1);
}

static int test_array(void) {
  printf("%s\n", __FUNCTION__);
  uint64_t ref[TEST_SIZE];
  uint8_t mem8[TEST_SIZE];
  uint16_t mem16[TEST_SIZE];
  uint32_t mem32[TEST_SIZE];
  uint64_t mem64[TEST_SIZE];
  
  bitmanio_array8_t ba8;
  bitmanio_array16_t ba16;
  bitmanio_array32_t ba32;
  bitmanio_array64_t ba64;

  uint32_t i;
  for (i = 0; i < sizeof(ref)/sizeof(ref[0]); i++) {
    ref[i] = (((((((uint64_t)rand()) << 16) ^ (uint64_t)rand()) << 16) ^ (uint64_t)rand()) << 16) ^ (uint64_t)rand();
  }

  uint32_t j;
  for (j = 1; j <= 64; j++) {
    bitmanio_init_array8(&ba8, mem8, j);
    bitmanio_init_array16(&ba16, mem16, j);
    bitmanio_init_array32(&ba32, mem32, j);
    bitmanio_init_array64(&ba64, mem64, j);

    for (i = 0; i < sizeof(ref)/sizeof(ref[0]); i++) {
      uint64_t v = ref[i] & ((1ULL<<j)-1);
      if (j <= 8) bitmanio_set8(&ba8, i, v);
      if (j <= 16) bitmanio_set16(&ba16, i, v);
      if (j <= 32) bitmanio_set32(&ba32, i, v);
      bitmanio_set64(&ba64, i, v);
    }

    for (i = 0; i < sizeof(ref)/sizeof(ref[0]); i++) {
      uint64_t v = ref[i] & ((1ULL<<j)-1);
      uint32_t v8 = j <= 8 ? bitmanio_get8(&ba8, i) : 0;
      uint32_t v16 = j <= 16 ? bitmanio_get16(&ba16, i) : 0;
      uint32_t v32 = j <= 32 ? bitmanio_get32(&ba32, i) : 0;
      uint64_t v64 = bitmanio_get64(&ba64, i);
      if (j <= 8 && v8 != v) {
        printf("@%02x: differ %x != exp %lx   8 %ib\n",i, v8, v, j);
        return -1;
      }
      if (j <= 16 && v16 != v) {
        printf("@%02x: differ %x != exp %lx  16 %ib\n",i, v16, v, j);
        return -1;
      }
      if (j <= 32 && v32 != v) {
        printf("@%02x: differ %x != exp %lx  32 %ib\n",i, v32, v, j);
        return -1;
      }
      if (v64 != v) {
        printf("@%02x: differ %lx != exp %lx  64 %ib\n",i, v64, v, j);
        return -1;
      }
    }
  }

  return 0;
}

#ifdef DEBUG

static void _dump_bits(uint8_t *m, uint32_t len) {
  while (len--) {
    int i;
    uint8_t v = *m++;
    for (i = 0; i < 8; i++) {
      printf("%d", v & (1<<(7-i)) ? 1 : 0);
    }
    printf(" ");
  }
  printf("\n");
}

static void debug(void) {
  uint8_t mem8[8];

  bitmanio_stream8_t bs8;
  memset(mem8,0xff,sizeof(mem8));
  bitmanio_init_stream8(&bs8, mem8);
  int i = 0;
  while (1) {
    printf("bits written %d\n", bitmanio_getpos8(&bs8));
    _dump_bits(mem8, sizeof(mem8));
    if (bitmanio_getpos8(&bs8) + (i+1)>= 8*sizeof(mem8))
      break;
    bitmanio_writez8(&bs8, i, (i+1));
    printf("wrote %d bits\n", i+1);
    i = (i+1) % 7;
  }
}

#endif

int main(int argc, char **args) {
  (void)argc; (void)args;

#ifdef DEBUG
  debug();
#endif

  srand(0x12345678);
  int res_test_stream_random = test_stream_random();
  int res_test_stream_nz = test_stream_nonzero();
  int res_test_stream_z = test_stream_zero();
  int res_test_array = test_array();

  int res = 
            res_test_stream_random |
            res_test_stream_nz |
            res_test_stream_z |
            res_test_array |
            0;

  if (res) {
    printf("test failed\n");
  } else {
    printf("TEST OK\n");
  }
  return res;
}
