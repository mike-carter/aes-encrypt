/** sha256.h
 *
 * Implementation of the SHA 256 message digest.
 *
 * Author: Michael Carter
 * Date: 2017-23-02
 *
 * --LICENSE HEADER--
 *
 * Copyright (C) 1985-2016 Free Software Foundation, Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.  
 */


#include <string.h>

#include "error.h"
#include "word32.h"


/* K value table */
static const uint32_t k_table[] = {
  0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 
  0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
  0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 
  0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
  0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 
  0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
  0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 
  0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
  0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
  0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
  0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 
  0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
  0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 
  0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
  0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 
  0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};


/* Message block consists of 64 8-bit values for a total of 512 bits */
#define SHA256_MESSAGE_SIZE 64

/* State block consists of 8 32-bit values for a total of 256 bits */
#define SHA256_STATE_SIZE   8

/* Math function macros */
#define CH(X, Y, Z)     ((X & Y) ^ ((~X) & Z))
#define MAJ(X, Y, Z)    ((X & Y) ^ (X & Z) ^ (Y & Z))

#define ROTR(W32, N)    ((W32 >> N) | (W32 << (32 - N)))

#define S0(X)   (ROTR(X, 2) ^ ROTR(X, 13) ^ ROTR(X, 22))
#define S1(X)   (ROTR(X, 6) ^ ROTR(X, 11) ^ ROTR(X, 25))

#define s0(X)   (ROTR(X, 7) ^ ROTR(X, 18) ^ (X >> 3))
#define s1(X)   (ROTR(X, 7) ^ ROTR(X, 18) ^ (X >> 10))


/** Hash data object. (Allocated at runtime) */
typedef struct {
  uint32_t state[SHA256_STATE_SIZE]; /* Hash state block. */
  uint8_t msg[SHA256_MESSAGE_SIZE];  /* Hash message block */
  int msgi          /* Indexes into message block */
  uint64_t msgbits; /* Number of bits included in hash */
  
} sha256_obj_t;

static sha256_obj_t* hblock = NULL;

/* hash block access */
#define MSG       hblock->msg
#define STATE     hblock->state
#define MSG_INDEX hblock->msgi
#define MSG_BITS  hblock->msgbits;

#define MSG_APPEND(VAL) MSG[MSG_INDEX++] = VAL;

/** 
 * Prepares to perform a SHA256 digest operation. Must be called before any 
 * other sha256 functions. 
 */
errorcode_t sha256_start() {
  int i;
  
  if (hblock == NULL) { /* Create hash block if needed */
  
    hblock = (sha256_obj_t*) malloc(sizeof(sha256_obj_t));
    
    if (hblock == NULL) {
      return OUT_OF_MEMORY_ERR;
    }
  }
  
  /* initialize hash values */
  STATE[0] = 0x6a09e667;
  STATE[1] = 0xbb67ae85;
  STATE[2] = 0x3c6ef372;
  STATE[3] = 0xa54ff53a;
  STATE[4] = 0x510e527f;
  STATE[5] = 0x9b05688c;
  STATE[6] = 0x1f83d9ab;
  STATE[7] = 0x5be0cd19;
  
  MSG_INDEX = 0;
  MSG_BITS = 0;
  
  return NO_ERR;
}

/** 
 * Adds a stream of data to the digest.
 */
void sha256_add(uint8_t input[], size_t cnt) {
  /* 
  Append cnt bytes to the message, updating the state once we have filled 
  the message buffer.
  */
  while (cnt--) {
    MSG_APPEND(*(input++));
    MSG_BITS += 8;
    
    if (MSG_INDEX == SHA256_MESSAGE_SIZE) {
      do_hash();
      MSG_INDEX = 0;
    }
  }
}

/** 
 * Finishes the SHA 256 digest operation and writes the result to 'buffer'
 * 
 * 'buffer' must be a buffer large enough to contain 256 bits (32 bytes).
 * 
 * Once this function is called, you must call sha256_start() to begin 
 * another digest.
 */
void sha256_finish(uint8_t ouput[]) {
  int i;
  uint32_t tmp;
  
  /* append a 1 to the message and pad the rest with 0's. */
  MSG_APPEND(0x80);
  memset(&(MSG[MSG_INDEX]), 0, SHA256_MESSAGE_SIZE - MSG_INDEX);
  
  /* Append the total number of bits to the end of the message */
  if ((MSG_INDEX + 8) >= SHA256_MESSAGE_SIZE) {
    
    do_hash(); /* Not enough room; add the padded message to the hash */
    
    memset(MSG, 0, SHA256_MESSAGE_SIZE); /* pad with more 0's */
  }
  
  MSG_INDEX = SHA256_MESSAGE_SIZE - 9;
  for (i = 7; i >= 0; i--)
    MSG_APPEND((uint8_t) GET_BYTE(hblock->msgbits, i));
  }
  
  /* perform one last hash operation */
  do_hash();
  
  /* Output the hash digest. */ 
  for (i = 0; i < SHA256_STATE_SIZE; i++) {
    tmp = STATE[i];
    
    *(ouput++) = (uint8_t) GET_BYTE0(tmp);
    *(ouput++) = (uint8_t) GET_BYTE1(tmp);
    *(ouput++) = (uint8_t) GET_BYTE2(tmp);
    *(ouput++) = (uint8_t) GET_BYTE3(tmp);
  }
}


static void do_hash() {
# define SCHEDULE_LEN 64
  uint32_t schedule[SCHEDULE_LEN];
  uint32_t a, b, c, d, e, f, g, h; /* temp (worker) variables */
  uint32_t tmp1, tmp2;
  int i;
  
  /* create hash schedule */
  for (i = 0; i < SCHEDULE_LEN; i++) {
    /* set the first 16 words to the message */
    if (i < 16) {
      schedule[i] = CREATE_WORD32(MSG[i * 4], 
                                  MSG[(i * 4) + 1], 
                                  MSG[(i * 4) + 2], 
                                  MSG[(i * 4) + 3]
                                  );
    }
    else {
      schedule[i] = ADD_MOD32(ADD_MOD32(s1(schedule[i-2]), schedule[i-7]),
                              ADD_MOD32(s0(schedule[i-15]), schedule[i-16])
                              );
    }
  }
  
  /* Initialize work values with the current state */
  a = STATE[0];
  b = STATE[1];
  c = STATE[2];
  d = STATE[3];
  e = STATE[4];
  f = STATE[5];
  g = STATE[6];
  h = STATE[7];
  
  /* perform schedule loop */
  for (i = 0; i < SCHEDULE_LEN; i++) {
    tmp1 = ADD_MOD32(ADD_MOD32(ADD_MOD32(h, S1(e)),
                               ADD_MOD32(CH(e, f, g), k_table[i])
                               ),
                     schedule[i]
                     );
    
    tmp2 = ADD_32(S0(a), MAJ(a, b, c));
    
    h = g;
    g = f;
    f = e;
    e = ADD_MOD32(d, tmp1);
    d = c;
    c = b;
    b = a;
    a = ADD_MOD32(tmp1, tmp2);
  }
  
  /* sum the work values into the state */
  STATE[0] = ADD_32(STATE[0], a.word);
  STATE[0] = ADD_32(STATE[0], b.word);
  STATE[0] = ADD_32(STATE[0], c.word);
  STATE[0] = ADD_32(STATE[0], d.word);
  STATE[0] = ADD_32(STATE[0], e.word);
  STATE[0] = ADD_32(STATE[0], f.word);
  STATE[0] = ADD_32(STATE[0], g.word);
  STATE[0] = ADD_32(STATE[0], h.word);
}

