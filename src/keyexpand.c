#include <stdlib.h>
#include <string.h>

void 

void rot_word(char *word) {
  char temp;
  int i;
  for (i=0; i<3; i++) {
	temp = word[i];
	word[i] = word[i+1];
	word[i+1] = temp;
  }
}

char* expand_key_16(char *key) {
  char* exp_key, exp_key_ptr, key_ptr;
  
  exp_key = (char *)malloc((size_t)176);
  exp_key_ptr = exp_key;
  
  // Copy key into first 16 bytes of expanded key
  memcopy(exp_key, key, 
}

char* expand_key(char* key, key_type_t key_type)
{
  size_t exp_key_size;
  char *exp_key;
  char key_part
  
  switch (key_type)
  {
  case key_16_byte:
	exp_size = 176;
	break;
  case key_24_byte:
	exp_size = 208;
	break;
  case key_32_byte:
	exp_size = 240
	break;
  }

  exp_key = (char*)malloc(exp_size);

  // Copy
}
