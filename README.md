# bitmanio
[![Build Status](https://travis-ci.org/pellepl/bitmanio.svg?branch=master)](https://travis-ci.org/pellepl/bitmanio)
Bit manipulation IO - break free of the fixed bit size tyranny

 This library gives variable bit range access to memory, either as streams or as arrays. Streams will give dynamic bit range access while arrays give fixed bit range access.
 
 The underlying memory buffer can be defined as 8, 16, 32, or 64 bit arrays. Bitmanio can only handle bit ranges up to underlying memory size. If you compile bitmanio to tampering with e.g. 16 bit memory, the variable bit range will comprise from 1 to 16 bits. However, all variants can be exist simultaneously as the API will be named according to underlying structure.
 
 Compiling the 8-bit variant will declare functions:
```C
   void bitmanio_init_stream8(bitmanio_stream8_t *bs, uint8_t *mem);
   uint8_t bitmanio_read8(bitmanio_stream8_t *bs, uint8_t bits);
   ... etc
```
 while the 16-bit yields:
```C
   void bitmanio_init_stream16(bitmanio_stream16_t *bs, uint16_t *mem);
   uint16_t bitmanio_read16(bitmanio_stream16_t *bs, uint8_t bits);
   ... etc
``` 
 Bitmanio has an upper limit of 2^23 number of bytes in total for an
 array or stream.
 
 The header file contains both API and implementation. To include this library as both header and implementation in your source, define `BITMANIO_STORAGE_BITS` to either 8, 16, 32, or 64; and include the `bitmanio.h` file.
 
 To include only the function definitions, define `BITMANIO_HEADER` along with the number of storage bits before including `bitmanio.h`.
 
## Example 1 - use 8-bit variant in only one file:
```C
 //file:bitmanio_tester_8.c 
   #define BITMANIO_STORAGE_BITS 8
   #include "bitmanio.h"
   <.. code using the library ..>
```
## Example 2 - use 8-bit and 32-bit variants in only one file:
```C
 //file:bitmanio_tester_8_and_32.c 
   #define BITMANIO_STORAGE_BITS 8
   #include "bitmanio.h"
   #define BITMANIO_STORAGE_BITS 32
   #include "bitmanio.h"
   <.. code using the library ..>
```
## Example 3 - use 16-bit variants from two files:
```C
 //file:bitmanio_code.c
   #define BITMANIO_STORAGE_BITS 16
   #include "bitmanio.h"
   <EOF>
```
```C
 //file:bitstuffer_encoder.c
   #define BITMANIO_STORAGE_BITS 16
   #define BITMANIO_HEADER
   #include "bitmanio.h"
   <.. code using the library ..>
```
```C
 //file:bitreader_decoder.c
   #define BITMANIO_STORAGE_BITS 16
   #define BITMANIO_HEADER
   #include "bitmanio.h"
   <.. code using the library ..>
```
## Full API
```C
/**
 * Initializes bitmanio memory stream.
 * @param bs  pointer a bitmanio_stream<X>_t struct
 * @param mem pointer memory where stream is read from or written to.
 */
void bitmanio_init_stream<X>(bitmanio_stream<X>_t *bs, uint<X>_t *mem);

/**
 * Reads from bitmanio memory stream.
 * @param bs    pointer a bitmanio_stream<X>_t struct
 * @param bits  number of bits to read
 * @return read value from stream
 */
uint<X>_t bitmanio_read<X>(bitmanio_stream<X>_t *bs, uint8_t bits);

/**
 * Writes to bitmanio memory stream. Does not zero the bits prior to writing.
 * Hence, when using this function the stream memory must be zeroed by user.
 * @param bs   pointer a bitmanio_stream<X>_t struct
 * @param v    the value to write
 * @param bits number of bits to write
 */
void bitmanio_write<X>(bitmanio_stream<X>_t *bs, uint<X>_t v, uint8_t bits);

/**
 * Writes to bitmanio memory stream. Zeroes the bits prior to writing.
 * @param bs   pointer a bitmanio_stream<X>_t struct
 * @param v    the value to write
 * @param bits number of bits to write
 */
void bitmanio_writez<X>(bitmanio_stream<X>_t *bs, uint<X>_t v, uint8_t bits);

/**
 * Sets bitmanio memory stream position in bits.
 * @param bs   pointer a bitmanio_stream<X>_t struct
 * @param bits bit offset in stream
 */
void bitmanio_setpos<X>(bitmanio_stream<X>_t *bs, uint32_t bits);

/**
 * Gets bitmanio memory stream position in bits.
 * @param bs   pointer a bitmanio_stream<X>_t struct
 * @return bit offset in stream
 */
uint32_t bitmanio_getpos<X>(bitmanio_stream<X>_t *bs);



/**
 * Initializes bitmanio memory array.
 * @param ba   pointer a bitmanio_array<X>_t struct
 * @param mem  pointer memory where the array is stored
 * @param bits number of bits of one array element
 */
void bitmanio_init_array<X>(bitmanio_array<X>_t *ba, uint<X>_t *mem, uint8_t bits);

/**
 * Reads value from bitmanio memory array.
 * @param ba pointer a bitmanio_array<X>_t struct
 * @param ix the index in the array to read
 * @return read value from array
 */
uint<X>_t bitmanio_get<X>(bitmanio_array<X>_t *ba, uint32_t ix);

/**
 * Writes value to bitmanio memory array.
 * @param ba pointer a bitmanio_array<X>_t struct
 * @param ix the index in the array to write to
 * @param v  the value to write
 */
void bitmanio_set<X>(bitmanio_array<X>_t *ba, uint32_t ix, uint<X>_t v);
```



