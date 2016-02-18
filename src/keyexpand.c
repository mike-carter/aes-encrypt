/* Library includes */
#include <stdlib.h>
#include <string.h>

/* Project includes */
#include "aes.h"
#include "bytesub.h"

/* Symbolic Constants */
#define EXP_KEY_SIZE_16 176
#define EXP_KEY_SIZE_24 208
#define EXP_KEY_SIZE_32 240

/* Local variables */
static char *exp_key = NULL; /* Encryption Key buffer */

static key_type_t key_size = 0; /* Size of current key */

const int rcon_table[15] = /* Rcon value lookup */
{
  0x01000000,
  0x02000000,
  0x04000000,
  0x08000000,
  0x10000000,
  0x20000000,
  0x40000000,
  0x80000000,
  0x1B000000,
  0x36000000,
  0x6C000000,
  0xD8000000,
  0xAB000000,
  0x4D000000,
  0x9A000000
}

/* Left-rotates first 4 bytes in the given buffer  */
static void
rot_word(char *word)
{
  char temp;
  int i;
  for (i=0; i<3; i++) {
	temp = word[i];
	word[i] = word[i+1];
	word[i+1] = temp;
  }
}

/* Substitutes the bytes in the given word with their AES S-Box value  */
static void
sub_word(char *word)
{
  bytesub_encrypt(word, 4);
}

/* Returns 4 bytes of the Expanded Key after the specified offset */
static void
get_exp_key_bytes(void *dest_ptr, int offset) {
  char *key_ptr = &exp_key[offset];
  memcpy(dest_ptr, key_ptr, 4);
}

/* Expands a 16-byte key */
char *
expand_key_16(char *key) {
  int * exp_key_ptr
  int round = 0;
  int temp_word1, temp_word2;

  if (exp_key != NULL) {
	free(exp_key);
  }

  /* Allocate memory for expanded key (Make sure to free() afterwards).  */
  exp_key = (char*)malloc((size_t)EXP_KEY_SIZE_16);

  /* Copy the key into the first 16 bytes of expanded key.  */
  memcpy(exp_key_bytes, (const void*)key, 16);

  /* Use a separate pointer to iterate through expanded key */
  exp_key_bytes = (int*)(&exp_key[16]);

  get_exp_key_bytes(&temp_word2, 12);
  rot_word((char *)temp_word1);
  sub_word((char *)temp_word1);

  temp_word1 ^= rcon_table[0];

  get_exp_key_bytes(&temp_word2, 0);
  *exp_key_bytes = temp_word1 ^ temp_word2;
  
  for (round = 5; round < 44; round++)
  {
	exp_key_bytes++;
	
	if ((round % 4) != 0)
	{ 
	  get_exp_key_bytes(&temp_word1, ((round - 1) * 4));
	  get_exp_key_bytes(&temp_word2, ((round - 4) * 4));
	  *exp_key_bytes = temp_word1 ^ temp_word2;
	}
	else /* Run this every 4th round */
	{
	  get_exp_key_bytes(&temp_word1, ((round - 4) * 4));
	  rot_word((char *)temp_word1);
	  sub_word((char *)temp_word1);

	  temp_word1 ^= rcon_table[(round / 4) - 1];

	  get_exp_key_bytes(&temp_word2, ((round - 4) * 4));
	  *exp_key_bytes = temp_word1 ^ temp_word2;
	}
  }
  return exp_key;
}

char* expand_key(char* key, key_type_t key_type)
{ 
  switch (key_type)
  {
  case key_16_byte:
	break;
  case key_24_byte:
	break;
  case key_32_byte:
	break;
  }

  return exp_key;
}
