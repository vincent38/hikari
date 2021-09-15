/* 
 * Toy narrow-pipe 48-bit MD hash function 
 * PK - 2021
 */

#ifndef __MC48_H
#define __MC48_H

#include <stdint.h>

/*
 * The 6 IV bytes h[0]...h[5]
 */
#define IVB0 0xE9
#define IVB1 0x38
#define IVB2 0x05
#define IVB3 0x73
#define IVB4 0x74
#define IVB5 0x06

/* 
 * The Davies-Meyer compression function based on tcz48 using an XOR feedforward
 * Input m: a 128-bit message block
 * Input h: a 48-bit chaining value
 * Output: void, h is overwritten with the result
 */
void tcz48_dm(const uint8_t m[16], uint8_t h[6]);

/* 
 * Toy hash function with 48-bit digests
 * Input m: the message to be hashed, whose length is required to be an integral number of bytes
 * Input blen: the byte length of m
 * Input h: placeholder for the result. Must have been allocated by the caller.
 * Output: void, h is overwritten with the result
 */
void ht48(const uint8_t *m, uint64_t blen, uint8_t h[6]);

#endif
