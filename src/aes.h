/**
 * AES encryption algorithm header file 
 *
 * Author: Michael Carter 
 * 
 *
 */

#ifndef _AES_H_
#define _AES_H_

#include <stdint.h>

/* Number of bytes in an encryption block */
#define AES_BLOCK_SIZE 16

#define CIPHER_EXTENSION ".aes"


/*
--MACROS--

I made these macros to assist in making terminal output look a little cleaner.
*/
/* This macro allows me to put verbose messages on one line. */
#define VERBOSE(...) if (flags.verbose) printf( __VA_ARGS__ )

/* This macro prints an error message and quits */
#define exit_error(...) fprintf(stderr, __VA_ARGS__ ); exit(EXIT_FAILURE)

/* Type Definitions: */

typedef union {
  uint8_t byte[4];
  uint32_t word;
} word32_t;


/**
 * Key size options
 */
typedef enum
{
  key_16_bytes = 16,
  key_24_bytes = 24,
  key_32_bytes = 32
} key_size_t;

typedef struct
{
  key_size_t size; /* Size of the key */
  unsigned char block[32]; /* Normal key block */
  unsigned char exp_block[240]; /* Expanded key block */
} aes_key_t;

/* External functions */

/* Key Expansion Function. (imported from keyexpand.c) */
extern void key_expansion(aes_key_t *);

/* Imported from base64.c */
extern char * base64_encode(const uint8_t *,size_t,size_t*);
extern uint8_t * base64_decode(const char *,size_t,size_t*);

/* Imported from bytesub.c  */
extern void bytesub_encrypt(uint8_t *, size_t);

/* Imported from bytesub.c  */
extern void bytesub_decrypt(uint8_t *, size_t);

/* Imported from gf.c */
extern uint8_t ff_multiply(uint8_t, uint8_t);


#endif /* _AES_H_ */
