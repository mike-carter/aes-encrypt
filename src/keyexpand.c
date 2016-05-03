/* Library includes */
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* Project includes */
#include "aes.h"

/* External functions */
extern void bytesub_encrypt(uint8_t *block, size_t count);

/* Rcon lookup table. */
static const uint32_t rcon_table[15] = 
{
  0x01000000, 0x02000000, 0x04000000,
  0x08000000, 0x10000000, 0x20000000,
  0x40000000, 0x80000000, 0x1B000000,
  0x36000000, 0x6C000000, 0xD8000000,
  0xAB000000, 0x4D000000, 0x9A000000
};


/* Left-rotates the bytes in a word  */
static uint32_t rot_word(uint32_t word) {
  uint32_t temp = (word & 0xFF000000) >> 24;
  word = word << 8;
  word = word | temp;
  return word;
}

/* Substitutes the bytes in the given word with their AES S-Box value  */
static uint32_t sub_word(uint32_t word)
{
  bytesub_encrypt((uint8_t*)&word, 4);
  return word;
}

/**
 * Converts an AES encryption key into its expanded form
 * 
 * @param key Encryption key
 * @param key_type Size of encryption key
 */
void key_expansion(aes_key_t *key)
{
  int i, key_word_size, num_rounds;
  uint32_t temp;
  uint32_t *w_key = (uint32_t*)key->block;
  uint32_t *w_exp_key = (uint32_t*)key->exp_block;
  
  /* Determine the key's size in number of words */
  key_word_size = key->size / 4;

  /* Determine the number of rounds based on the key size */
  num_rounds = key_word_size + 6;

  /* Copy key into the beginning of the expanded key buffer */
  for (i = 0; i < key_word_size; i++) {
	w_exp_key[i] = w_key[i];
  }

  /* Perform expansion */
  for (i = key_word_size; i < (AES_BLOCK_SIZE/4)*(num_rounds+1); i++) {
	temp = w_exp_key[i-1];
	
	if ((i % key_word_size) == 0)
	  temp = sub_word(rot_word(temp)) ^ rcon_table[i/key_word_size];
	  
	else if ((key_word_size > 6) && ((i % key_word_size) == 4))
	  temp = sub_word(temp);

	w_exp_key[i] = w_exp_key[i - key_word_size] ^ temp;
  }
}
