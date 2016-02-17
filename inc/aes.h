/* AES encryption algorithm header file 
 *
 * Michael Carter 
 * 
 *
 */

#ifndef _AES_H_
#define _AES_H_

#include <stdlib.h>
#include <stdbool.h>

#define BLOCK_SIZE 16;


typedef char block_t[BLOCK_SIZE];

typedef enum {
  key_16_byte,
  key_24_byte,
  key_32_byte
} key_type_t;


#endif /* _AES_H_ */
