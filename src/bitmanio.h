//The MIT License (MIT)
//
//Copyright (c) 2017 Peter Andersson (pelleplutt1976<at>gmail.com)
//
//Permission is hereby granted, free of charge, to any person obtaining a copy of
//this software and associated documentation files (the "Software"), to deal in
//the Software without restriction, including without limitation the rights to
//use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
//the Software, and to permit persons to whom the Software is furnished to do so,
//subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
//FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
//COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
//IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
//CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


/**
 * bitmanio.h
 * Bit manipulation IO - break free of the fixed bit size tyranny
 *
 * This library gives variable bit range access to memory, either as
 * streams or as arrays. Streams will give dynamic bit range access
 * while arrays give fixed bit range access.
 *
 * The underlying memory buffer can be defined as 8, 16, 32, or 64
 * bit arrays. Bitmanio can only handle bit ranges up to underlying
 * memory size. If you compile bitmanio to tampering with e.g. 16 bit
 * memory, the variable bit range will comprise from 1 to 16 bits.
 * However, all variants can be exist simultaneously as the API will
 * be named according to underlying structure.
 *
 * Compiling the 8-bit variant will declare functions:
 *   void bitmanio_init_stream8(bitmanio_stream8_t *bs, uint8_t *mem);
 *   uint8_t bitmanio_read8(bitmanio_stream8_t *bs, uint8_t bits);
 *   ... etc
 * while the 16-bit yields:
 *   void bitmanio_init_stream16(bitmanio_stream16_t *bs, uint16_t *mem);
 *   uint16_t bitmanio_read16(bitmanio_stream16_t *bs, uint8_t bits);
 *   ... etc
 *
 * Bitmanio has an upper limit of 2^23 number of bytes in total for an
 * array or stream.
 *
 * The header file contains both API and implementation.
 * To include this library as both header and implementation in your
 * source, define
 *   BITMANIO_STORAGE_BITS
 * to either 8, 16, 32, or 64; and include the bitmanio.h file.
 *
 * To include only the function definitions, define
 *   BITMANIO_HEADER
 * along with the number of storage bits before including bitmanio.h.
 *
 * Example 1 - use 8-bit variant in only one file:
 * //file:bitmanio_tester_8.c
 *   #define BITMANIO_STORAGE_BITS 8
 *   #include "bitmanio.h"
 *   <.. code using the library ..>
 *
 * Example 2 - use 8-bit and 32-bit variants in only one file:
 * //file:bitmanio_tester_8_and_32.c
 *   #define BITMANIO_STORAGE_BITS 8
 *   #include "bitmanio.h"
 *   #define BITMANIO_STORAGE_BITS 32
 *   #include "bitmanio.h"
 *   <.. code using the library ..>
 *
 * Example 3 - use 16-bit variants from two files:
 * //file:bitmanio_code.c
 *   #define BITMANIO_STORAGE_BITS 16
 *   #include "bitmanio.h"
 *   <EOF>
 * //file:bitstuffer_encoder.c
 *   #define BITMANIO_STORAGE_BITS 16
 *   #define BITMANIO_HEADER
 *   #include "bitmanio.h"
 *   <.. code using the library ..>
 * //file:bitreader_decoder.c
 *   #define BITMANIO_STORAGE_BITS 16
 *   #define BITMANIO_HEADER
 *   #include "bitmanio.h"
 *   <.. code using the library ..>
 */

#undef __BM_TYPE
#undef __BM_FN_PF
#if   BITMANIO_STORAGE_BITS == 8
  #define __BM_TYPE uint8_t
  #define __BM_FN_PF 8
#elif BITMANIO_STORAGE_BITS == 16
  #define __BM_TYPE uint16_t
  #define __BM_FN_PF 16
#elif BITMANIO_STORAGE_BITS == 32
  #define __BM_TYPE uint32_t
  #define __BM_FN_PF 32
#elif BITMANIO_STORAGE_BITS == 64
  #define __BM_TYPE uint64_t
  #define __BM_FN_PF 64
#elif BITMANIO_STORAGE_BITS == BYTE
#else
  #error BITMANIO_STORAGE_BITS must be 8, 16, 32, 64, or BYTE
#endif

#undef  __BM_FILTERED
#define __BM_FILTERED             0

/* inclusion guard */
#ifndef __BM_IMPLEMENTATION
  #if   BITMANIO_STORAGE_BITS ==  8 && !defined(__BITMANIO8_H_)
   #define __BITMANIO8_H_
  #elif BITMANIO_STORAGE_BITS == 16 && !defined(__BITMANIO16_H_)
    #define __BITMANIO16_H_
  #elif BITMANIO_STORAGE_BITS == 32 && !defined(__BITMANIO32_H_)
    #define __BITMANIO32_H_
  #elif BITMANIO_STORAGE_BITS == 64 && !defined(__BITMANIO64_H_)
    #define __BITMANIO64_H_
  #elif BITMANIO_STORAGE_BITS == BYTE && !defined(__BITMANIOBYTE_H_)
    #define __BITMANIOBYTE_H_
  #else
    #undef __BM_FILTERED
    #define __BM_FILTERED         1
  #endif
#endif /* __BM_IMPLEMENTATION */

#if __BM_FILTERED == 0

#define __BM_FN_I(a,b) a##b
#define __BM_TP_I(a,b) a##b##_t
#define __BM_FN(a,b) __BM_FN_I(a, b)
#define __BM_TP(a,b) __BM_TP_I(a, b)

#undef __BM_TBITS
#define __BM_TBITS (sizeof(__BM_TYPE)*8)

#ifndef __BM_IMPLEMENTATION
#if BITMANIO_STORAGE_BITS != BYTE
/**
 * bitmanio memory stream
 */
typedef struct {
  /** memory pointer */
  __BM_TYPE *mem;
  /** current memory offset */
  uint32_t m_offs;
  /** current bit offset */
  uint8_t b_offs;
} __BM_TP(bitmanio_stream, __BM_FN_PF);

/**
 * bitmanio memory array
 */
typedef struct {
  /** memory pointer */
  __BM_TYPE *mem;
  /** number of bits in array entries */
  uint8_t bits;
} __BM_TP(bitmanio_array, __BM_FN_PF);

#else /* BITMANIO_STORAGE_BITS == BYTE */
/**
 * bitmanio memory array, underlying 8-bit organization
 */
typedef struct {
  /** memory pointer */
  uint8_t *mem;
  /** number of bits in array entries */
  uint8_t bits;
} bitmanio_bytearray_t;

/**
 * bitmanio memory stream, underlying 8-bit organization
 */
typedef struct {
  /** memory pointer */
  uint8_t *mem;
  /** number of bits in array entries */
  uint32_t bitpos;
} bitmanio_bytestream_t;

#endif /* BITMANIO_STORAGE_BITS == BYTE */
#endif /* __BM_IMPLEMENTATION */


#if BITMANIO_STORAGE_BITS != BYTE

/**
 * Initializes bitmanio memory stream.
 * @param bs  pointer a bitmanio_stream<X>_t struct
 * @param mem pointer memory where stream is read from or written to.
 */
void __BM_FN(bitmanio_init_stream, __BM_FN_PF)(
  __BM_TP(bitmanio_stream, __BM_FN_PF) *bs,
  __BM_TYPE *mem)
#ifndef __BM_IMPLEMENTATION
;
#else
{
  bs->mem = mem;
  bs->m_offs = 0;
  bs->b_offs = 0;
}
#endif /* __BM_IMPLEMENTATION */

/**
 * Reads from bitmanio memory stream.
 * @param bs    pointer a bitmanio_stream<X>_t struct
 * @param bits  number of bits to read
 * @return read value from stream
 */
__BM_TYPE __BM_FN(bitmanio_read, __BM_FN_PF)(
  __BM_TP(bitmanio_stream, __BM_FN_PF) *bs,
  uint8_t bits)
#ifndef __BM_IMPLEMENTATION
;
#else
{
  __BM_TYPE v = 0;
  __BM_TYPE *d = &bs->mem[bs->m_offs];
  if (bs->b_offs + bits > __BM_TBITS) {
    uint8_t shift = 2 * __BM_TBITS - (bs->b_offs + bits);
    v = (d[1] >> shift) | (d[0] << (__BM_TBITS - shift));
  } else {
    v = d[0] >> (__BM_TBITS - (bits + bs->b_offs));
  }
  bs->b_offs += bits;
  if (bs->b_offs >= __BM_TBITS) {
    bs->b_offs &= (__BM_TBITS - 1);
    bs->m_offs++;
  }
  return bits == __BM_TBITS ? v : v & (((__BM_TYPE)1<<bits) - 1);
}
#endif /* __BM_IMPLEMENTATION */

/**
 * Writes to bitmanio memory stream. Does not zero the bits prior to writing.
 * Hence, when using this function the stream memory must be zeroed by user.
 * @param bs   pointer a bitmanio_stream<X>_t struct
 * @param v    the value to write
 * @param bits number of bits to write
 */
void __BM_FN(bitmanio_write, __BM_FN_PF)(
  __BM_TP(bitmanio_stream, __BM_FN_PF) *bs,
  __BM_TYPE v,
  uint8_t bits)
#ifndef __BM_IMPLEMENTATION
;
#else
{
  v &= bits == __BM_TBITS ? (__BM_TYPE)~0 : (__BM_TYPE)(((__BM_TYPE)1<<bits) - 1);
  __BM_TYPE *d = &bs->mem[bs->m_offs];
  if (bs->b_offs + bits > __BM_TBITS) {
    uint8_t shift = 2 * __BM_TBITS - (bs->b_offs + bits);
    d[1] |= v << shift;
    d[0] |= v >> (__BM_TBITS - shift);
  } else {
    d[0] |= v << (__BM_TBITS - (bits + bs->b_offs));
  }
  bs->b_offs += bits;
  if (bs->b_offs >= __BM_TBITS) {
    bs->b_offs &= (__BM_TBITS - 1);
    bs->m_offs++;
  }
}
#endif /* __BM_IMPLEMENTATION */

/**
 * Writes to bitmanio memory stream. Zeroes the bits prior to writing.
 * @param bs   pointer a bitmanio_stream<X>_t struct
 * @param v    the value to write
 * @param bits number of bits to write
 */
void __BM_FN(bitmanio_writez, __BM_FN_PF)(
  __BM_TP(bitmanio_stream, __BM_FN_PF) *bs,
  __BM_TYPE v,
  uint8_t bits)
#ifndef __BM_IMPLEMENTATION
;
#else
{
  __BM_TYPE m = bits == __BM_TBITS ? (__BM_TYPE)~0 : (__BM_TYPE)(((__BM_TYPE)1<<bits)-1);
  v &= m;
  __BM_TYPE *d = &bs->mem[bs->m_offs];
  if (bs->b_offs + bits > __BM_TBITS) {
    uint8_t shift = 2 * __BM_TBITS - (bs->b_offs + bits);
    d[1] = (d[1] & ~(m << shift)) | (v << shift);
    d[0] = (d[0] & ~(m >> (__BM_TBITS - shift))) | (v >> (__BM_TBITS - shift));
  } else {
    uint8_t shift = __BM_TBITS - (bits + bs->b_offs);
    d[0] = (d[0] & ~(m << shift)) | (v << shift);
  }
  bs->b_offs += bits;
  if (bs->b_offs >= __BM_TBITS) {
    bs->b_offs &= (__BM_TBITS - 1);
    bs->m_offs++;
  }
}
#endif /* __BM_IMPLEMENTATION */

/**
 * Sets bitmanio memory stream position in bits.
 * @param bs   pointer a bitmanio_stream<X>_t struct
 * @param bits bit offset in stream
 */
void __BM_FN(bitmanio_setpos, __BM_FN_PF)(
  __BM_TP(bitmanio_stream, __BM_FN_PF) *bs,
  uint32_t bits)
#ifndef __BM_IMPLEMENTATION
;
#else
{
  bs->m_offs = bits / (__BM_TBITS);
  bs->b_offs = bits & (__BM_TBITS - 1);
}
#endif /* __BM_IMPLEMENTATION */

/**
 * Gets bitmanio memory stream position in bits.
 * @param bs   pointer a bitmanio_stream<X>_t struct
 * @return bit offset in stream
 */
uint32_t __BM_FN(bitmanio_getpos, __BM_FN_PF)(
  __BM_TP(bitmanio_stream, __BM_FN_PF) *bs)
#ifndef __BM_IMPLEMENTATION
;
#else
{
  return bs->m_offs * __BM_TBITS + bs->b_offs;
}
#endif /* __BM_IMPLEMENTATION */


/**
 * Initializes bitmanio memory array.
 * @param ba   pointer a bitmanio_array<X>_t struct
 * @param mem  pointer memory where the array is stored
 * @param bits number of bits of one array element
 */
void __BM_FN(bitmanio_init_array, __BM_FN_PF)(
  __BM_TP(bitmanio_array, __BM_FN_PF) *ba,
  __BM_TYPE *mem,
  uint8_t bits)
#ifndef __BM_IMPLEMENTATION
;
#else
{
  ba->mem = mem;
  ba->bits = bits;
}
#endif /* __BM_IMPLEMENTATION */

/**
 * Reads value from bitmanio memory array.
 * @param ba pointer a bitmanio_array<X>_t struct
 * @param ix the index in the array to read
 * @return read value from array
 */
__BM_TYPE __BM_FN(bitmanio_get, __BM_FN_PF)(
  __BM_TP(bitmanio_array, __BM_FN_PF) *ba,
  uint32_t ix)
#ifndef __BM_IMPLEMENTATION
;
#else
{
  uint8_t bits = ba->bits;
  uint8_t b_offs = (ix * bits) & (__BM_TBITS - 1);
  __BM_TYPE *d = &ba->mem[(ix * bits) / __BM_TBITS];
  __BM_TYPE v;
  if (b_offs + bits > __BM_TBITS) {
    uint8_t bl = 2 * __BM_TBITS - (b_offs + bits);
    v = (d[1] >> bl) | (d[0] << (__BM_TBITS - bl));
  } else {
    v = d[0] >> (__BM_TBITS - (bits + b_offs));
  }
  return bits == __BM_TBITS ? v : v & (((__BM_TYPE)1<<bits) - 1);
}
#endif /* __BM_IMPLEMENTATION */

/**
 * Writes value to bitmanio memory array.
 * @param ba pointer a bitmanio_array<X>_t struct
 * @param ix the index in the array to write to
 * @param v  the value to write
 */
void __BM_FN(bitmanio_set, __BM_FN_PF)(
  __BM_TP(bitmanio_array, __BM_FN_PF) *ba,
  uint32_t ix,
  __BM_TYPE v)
#ifndef __BM_IMPLEMENTATION
;
#else
{
  uint8_t bits = ba->bits;
  uint32_t ix_mul_bits = ix * bits;
  uint8_t b_offs = ix_mul_bits & (__BM_TBITS - 1);
  __BM_TYPE *d = &ba->mem[ix_mul_bits / __BM_TBITS];
  __BM_TYPE m = bits == __BM_TBITS ? (__BM_TYPE)~0 : (__BM_TYPE)(((__BM_TYPE)1<<bits)-1);
  v &= m;
  if (b_offs + bits > __BM_TBITS) {
    uint8_t shift = 2 * __BM_TBITS - (b_offs + bits);
    d[1] = (d[1] & ~(m << shift)) | (v << shift);
    d[0] = (d[0] & ~(m >> (__BM_TBITS - shift))) | (v >> (__BM_TBITS - shift));
  } else {
    uint8_t shift = __BM_TBITS - (bits + b_offs);
    d[0] = (d[0] & ~(m << shift)) | (v << shift);
  }
}
#endif /* __BM_IMPLEMENTATION */

#else /* BITMANIO_STORAGE_BITS == BYTE */

#ifdef __BM_IMPLEMENTATION
static uint32_t __bm_bits_get(uint8_t *mem, uint32_t bitix,
    uint8_t bits) {
  uint8_t rbits = bits;
  uint32_t mix = bitix / 8;
  uint32_t sbix = bitix % 8;

  uint32_t v;

  uint8_t mask;
  uint8_t fbits = 8 - sbix;
  /* first do the byte with possible offset */
  if (rbits < fbits) {
    mask = (1<<rbits)-1;
    v = (mem[mix] >> sbix) & mask;
    rbits = 0;
  } else {
    mask = (1<<fbits)-1;
    v = (mem[mix] >> sbix) & mask;
    rbits -= fbits;
  }
  mix++;
  /* then do the bytes guaranteed no offset and full (fbits=8,rbits>=8) */
  while (rbits >= 8) {
    v |= (mem[mix]) << (bits - rbits);
    rbits -= 8;
    mix++;
  }
  /* finally the no-offset, non-full byte (fbits=8,rbits<8) */
  if (rbits) {
    mask = (1<<rbits)-1;
    v |= (mem[mix] & mask) << (bits - rbits);
  }

  return v;
}

static void __bm_bits_set(uint8_t *mem, uint32_t bitix,
    uint8_t bits, uint32_t v) {
  uint8_t rbits = bits;
  uint32_t mix = bitix / 8;
  uint32_t sbix = bitix % 8;

  uint8_t mask;
  uint8_t data;
  uint8_t fbits = 8 - sbix;
  /* first do the byte with possible offset */
  if (rbits < fbits) {
    mask = (1<<rbits)-1;
    data = (v & mask) << sbix;
    mask <<= sbix;
    rbits = 0;
  } else {
    mask = (1<<fbits)-1;
    data = (v & mask) << sbix;
    mask <<= sbix;
    rbits -= fbits;
    v >>= fbits;
  }
  mem[mix] = (mem[mix] & ~mask) | data;
  mix++;
  /* then do the bytes guaranteed no offset and full (fbits=8,rbits>=8) */
  while (rbits >= 8) {
    mem[mix] = v;
    rbits -= 8;
    v >>= 8;
    mix++;
  }
  /* finally the no-offset, non-full byte (fbits=8,rbits<8) */
  if (rbits) {
    mask = (1<<rbits)-1;
    data = (v & mask);
    mem[mix] = (mem[mix] & ~mask) | data;
  }
}
#endif /* __BM_IMPLEMENTATION */

/**
 * Initializes bitmanio memory bytestream.
 * @param bs  pointer a bitmanio_bytestream_t struct
 * @param mem pointer memory where stream is read from or written to.
 */
void bitmanio_init_stream(bitmanio_bytestream_t *bs,
    uint8_t *mem)
#ifndef __BM_IMPLEMENTATION
;
#else
{
  bs->mem = mem;
  bs->bitpos = 0;
}
#endif /* __BM_IMPLEMENTATION */

/**
 * Reads from bitmanio memory bytestream.
 * @param bs    pointer a bitmanio_bytestream_t struct
 * @param bits  number of bits to read
 * @return read value from stream
 */
uint32_t bitmanio_read(bitmanio_bytestream_t *bs,
  uint8_t bits)
#ifndef __BM_IMPLEMENTATION
;
#else
{
  uint32_t v = __bm_bits_get(bs->mem, bs->bitpos, bits);
  bs->bitpos += bits;
  return v;
}
#endif /* __BM_IMPLEMENTATION */

/**
 * Writes to bitmanio memory bytestream.
 * @param bs   pointer a bitmanio_bytestream_t struct
 * @param v    the value to write
 * @param bits number of bits to write
 */
void bitmanio_write(bitmanio_bytestream_t *bs, uint32_t v,
  uint8_t bits)
#ifndef __BM_IMPLEMENTATION
;
#else
{
  __bm_bits_set(bs->mem, bs->bitpos, bits, v);
  bs->bitpos += bits;
}
#endif /* __BM_IMPLEMENTATION */

/**
 * Sets bitmanio memory bytestream position in bits.
 * @param bs   pointer a bitmanio_bytestream_t struct
 * @param bits bit offset in stream
 */
void bitmanio_setpos(bitmanio_bytestream_t *bs,
  uint32_t bits)
#ifndef __BM_IMPLEMENTATION
;
#else
{
  bs->bitpos = bits;
}
#endif /* __BM_IMPLEMENTATION */

/**
 * Gets bitmanio memory bytestream position in bits.
 * @param bs   pointer a bitmanio_bytestream_t struct
 * @return bit offset in stream
 */
uint32_t bitmanio_getpos(bitmanio_bytestream_t *bs)
#ifndef __BM_IMPLEMENTATION
;
#else
{
  return bs->bitpos;
}
#endif /* __BM_IMPLEMENTATION */

/**
 * Initializes bitmanio memory bytearray.
 * This differs from bitmanio_array<X> that the underlying memory always is
 * byte aligned. However, it does handle up to 32-bit array elements. It is not
 * as effective as bitmanio_array<X>_t:s, though.
 * Useful if the program must be agnostic to target endianness.
 * @param ba   pointer a bitmanio_bytearray_t struct
 * @param mem  pointer memory where the array is stored
 * @param bits number of bits of one array element
 */
void bitmanio_init_array(bitmanio_bytearray_t *ba,
    uint8_t *mem, uint8_t bits)
#ifndef __BM_IMPLEMENTATION
;
#else
{
  ba->mem = mem;
  ba->bits = bits;
}
#endif /* __BM_IMPLEMENTATION */

/**
 * Reads value from bitmanio memory bytearray.
 * @param ba pointer a bitmanio_bytearray_t struct
 * @param ix the index in the array to read
 * @return read value from array
 */
uint32_t bitmanio_get(bitmanio_bytearray_t *ba, uint32_t ix)
#ifndef __BM_IMPLEMENTATION
;
#else
{
  return __bm_bits_get(ba->mem, ba->bits * ix, ba->bits);
}
#endif /* __BM_IMPLEMENTATION */

/**
 * Writes value to bitmanio memory bytearray.
 * @param ba pointer a bitmanio_bytearray_t struct
 * @param ix the index in the array to write to
 * @param v  the value to write
 */
void bitmanio_set(bitmanio_bytearray_t *ba, uint32_t ix, uint32_t v)
#ifndef __BM_IMPLEMENTATION
;
#else
{
  __bm_bits_set(ba->mem, ba->bits * ix, ba->bits, v);
}
#endif /* __BM_IMPLEMENTATION */

#endif /* BITMANIO_STORAGE_BITS == BYTE */

#endif /* __BM_UNFILTERED */

#if !defined(__BM_IMPLEMENTATION)
  #ifndef BITMANIO_HEADER
    #define __BM_IMPLEMENTATION
    /* implement functions */
    #ifndef BITMANIO_H_WHEREABOUTS
      #define BITMANIO_H_WHEREABOUTS __FILE__
    #endif
    #include BITMANIO_H_WHEREABOUTS
    #undef __BM_IMPLEMENTATION
  #endif /* BITMANIO_HEADER */
  #undef BITMANIO_HEADER
#endif /* __BM_IMPLEMENTATION */


#undef BITMANIO_STORAGE_BITS
