/* 'aes-encrypt' file encryption program for GNU Linux

   Author: Michael Carter
   Date: 2016-05-02

   --LICENSE HEADER--

   Copyright (C) 1985-2016 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */
/* Name of program */
#define PROGRAM_NAME "aes-encrypt"

//#define DEBUG

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <time.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdint.h>
#include <sys/stat.h>

#include "aes.h"

#define DEFAULT_KEY_FILE "./aes.key"

#define DEFAULT_OUT_FILE "cipher"

#define OUTPUT_EXTENSION ".aes"

//#define DEFAULT_OUTPUT "cipher.aes"

/*
  --PROGRAM OPTIONS--

  There were a couple options that I wanted to implement, but due to time 
  constraints, I decided to not add them into the submitted version. 

  I figured that the --force and --burn-after-reading were not really 
  necessary to the actual implementation of the AES algorithm, ultimately,
  they were just some ideas I had for an actual software product that I could
  distribute. I may go back and implement them when I feel the urge to 
  continue working on this program. 

 */
/* Option flags structure */
struct Options {
  //bool force; /* -f flag: chmod files if necessary */
  //bool burn_file; /* -b flag: shred file after encrypting */
  bool verbose; /* -v flag: print progress */
  char * out_directory;
  char * key_file_name;
  key_size_t key_size;
};

/* Program options */
const struct option long_opts[] = {
  //{"force", no_argument, NULL, 'f'},
  //{"burn-after-read", no_argument, NULL, 'b'},
  {"verbose", no_argument, NULL, 'v'},
  {"out_dir", required_argument, NULL, 'd'},
  {"key_file_name", required_argument, NULL, 'k'},
  {"key_size", required_argument, NULL, 's'},
  {0, 0, 0, 0}
};
const char opts_str[] = "vd:k:s:";


/* 
  --GLOBAL VARIABLES--
  
  There are just 2 global variables in both the encrypt and decrypt programs.
  'flags' is the global flags structure that contains the options that the user
  selected.
  
  I tried to keep the key struct non-global, but I ran into issues with running 
  the key expansion algorithm when it was heap-allocated. My best guess is that
  it's because the code for the key expansion is in another file. 
*/

static struct Options flags; /* User Selected Options */

static aes_key_t encrypt_key; /* Contains the encryption key */

/*
  --FUNCTIONS--
 */

/**
 * This function creates a pseudo-random key that will be used to encrypt the 
 * given file. We also perform a key expansion that converts the 128/192/256
 * bit key into a unique sequence of 178/204/240 bytes that will eventually
 * be XOR'ed against every 16 byte sequence of the file.
 *
 * The details of the key expansion algorithm are described in keyexpand.c
 *
 * @param key - Pointer to the key structure that will be initialized.
 * @param key_size - Size of the key to construct. If we specify 0, then we use
 *                   the default key size (256 bits).
 */
void key_init(aes_key_t *key, int key_size) {
  int i;

  /* 
   We default to using a 256-bit key. Otherwise we use whatever strength 
   the user specified
   */
  if (key_size == 0) {
	VERBOSE("Size not specified. Using 256-bit encryption.\n");
	key->size = key_32_bytes;
  }
  else {
	key->size = key_size;
  }

#ifdef DEBUG
  /* 
   When creating aes-decrypt, I wanted to be able to control the key used, so I 
   wrote in this little segment
  */
  for (i=0; i < key->size; i++) {
	key->block[i] = 'a';
  }
#else
  
  uint8_t rand_byte;
  
  /* 
   Here, we generate the encryption key using random numbers using the current
   UNIX time as the generating seed.
   */
  srand(time(NULL));
  for (i = 0; i < key->size; i++) {
	rand_byte = rand() & 0xFF; 
	key->block[i] = rand_byte;
  }
  
#endif
  
  key_expansion(key); /* finish by running the key expansion algorithm. */

}

/**
 * This function saves the key to a plain-text file using base-64 encoding.
 * We will need this file when we later use this key to decrypt the file.
 *
 * @param key - Pointer to the encryption key
 * @param keyfd - File descriptor of a file that has been opened for writing. 
 */
void save_key(aes_key_t *key, FILE *keyfd) {
  size_t enc_len;
  char *b64_str;

  b64_str = base64_encode(key->block, (size_t)key->size, &enc_len);
  if (fputs(b64_str, keyfd) == EOF) {
	exit_error(PROGRAM_NAME ": Failed to write key to file.\n");
  }
}

/**
 * Performs the Sub Bytes function of the AES cipher, in which we replace each 
 * byte of the state with its corresponding S-Box value. For AES, the S-Box is 
 * a simple bijection, the pseudo-code for which is such:
 *
 * b[i] => The i'th bit of byte b
 *
 * byte f(byte b):
 *   unsigned char c = 0x63;
 *   if (b != 0):
 *     b = multiplicativeInverse(b);
 *   for (i = 0; i < 8; i++):
 *     b[i] = b[i] XOR b[(i+4) MOD 8] XOR b[(i+5) MOD 8] XOR
 *            b[(i+6) MOD 8] XOR b[(i+7) MOD 8] XOR c[i];
 *    return b;
 *
 * To increase computation speed, however, we simply put every S-Box value in
 * a table and simply perform a lookup. This table can be found in "bytesub.c".
 */
void sub_bytes(uint8_t **state) {
  int i;
  for (i = 0; i < 4; i++)
	bytesub_encrypt(state[i], 4);
}


/**
 * Shift Rows function of AES cipher.
 * This function simply performs a byte-wise rotation of the rows in the
 * state matrix. Each row is rotated a certian number of times.
 *
 * Row 0 is not affected.
 * Row 1 is rotated 1 time.
 * Row 2 is rotated 2 times.
 * Row 3 is rotated 3 times. 
 *
 * For encryption, we rotate right. When we decrypt, we rotate to the left.
 */
void shift_rows(uint8_t **state) {
  uint8_t temp;
  int r, i, c;
  
  for (r = 1; r < 4; r++) {
	for (i = 0; i < r; i++) {
	  for (c = 1; c < 4; c++) {
		temp = state[r][c-1];
		state[r][c-1] = state[r][c];
		state[r][c] = temp;
	  }
	}
  }
}


/**
 * Mix Columns function of AES algorithm
 *
 * Programmically, this function is fairly simple, however the math behind it
 * is rather complex. 
 *
 * At its core, we perform a matrix multiplication of the state matrix with the
 * following byte matrix:
 * [0x02 0x03 0x01 0x01]
 * [0x01 0x02 0x03 0x01]
 * [0x01 0x01 0x02 0x03]
 * [0x03 0x01 0x01 0x02]
 *
 * This multiplication is performed over a Finite Field, specifically a Galois
 * Field of order 256. A description of this Galois Field can be found in "The
 * Laws of Cryptology", page 119. Again, however, when implementing this 
 * multiplication we can use lookup tables to make the calculations much faster.
 * The actual implementation of this finite field multiplication can be found 
 * in "gf.c".
 */
void mix_columns(uint8_t **state) {
  int c;
  uint8_t temp[4];
  for (c = 0; c < 4; c++) {
	temp[0] = ff_multiply(0x02, state[0][c])
	  ^ ff_multiply(0x03, state[1][c])
	  ^ state[2][c]
	  ^ state[3][c];
	temp[1] = state[0][c]
	  ^ ff_multiply(0x02, state[1][c])
	  ^ ff_multiply(0x03, state[2][c])
	  ^ state[3][c];
	temp[2] = state[0][c]
	  ^ state[1][c]
	  ^ ff_multiply(0x02, state[2][c])
	  ^ ff_multiply(0x03, state[3][c]);
	temp[3] = ff_multiply(0x03, state[0][c])
	  ^ state[1][c]
	  ^ state[2][c]
	  ^ ff_multiply(0x02, state[3][c]);

	state[0][c] = temp[0];
	state[1][c] = temp[1];
	state[2][c] = temp[2];
	state[3][c] = temp[3];
  }
}

/**
 * Add Round Key function of the AES cipher.
 * 
 * Here is where we acutally apply the key in the cipher algorithm. During
 * each round of encryption, we XOR each byte of the 16-byte block against
 * the next 16 bytes of the expanded key. Once we've used that set of bytes,
 * we XOR the current state against the next 16 bytes, and never touch those
 * bytes again for this state. 
 * 
 */
void add_round_key(uint8_t **state, int round, uint8_t *exp_key) {
  int r, c;
  uint8_t keyind = 16 * round;

  for (c = 0; c < 4; c++) {
	for (r = 0; r < 4; r++) {
	  state[r][c] ^= exp_key[keyind++];
	}
  }
}

/**
 * This function is the overall application of the AES cipher in the context of
 * a single state block matrix.
 *
 * For each state, we perform the four AES functions several times, in what are
 * referred to as "rounds". The number of rounds we do depends on the size of 
 * the encryption key. 
 */
void aes_cipher(uint8_t **state, aes_key_t *key) {
  int round, num_rounds;

  num_rounds = (key->size / 4) + 6;
  
  add_round_key(state, 0, key->exp_block);

  for (round = 1; round < num_rounds; round++) {
	sub_bytes(state);
	shift_rows(state);
	mix_columns(state);
	add_round_key(state, round, key->exp_block);
  }

  /* Don't mix columns on the last round */
  sub_bytes(state);
  shift_rows(state);
  add_round_key(state, num_rounds, key->exp_block);
}

/**
 * This function reads the plaintext file in blocks of 16-bytes, and loads the
 * bytes into the state block. This state block is a 4x4 matrix, however, while
 * the array is row-major order, the bytes are arranged vertically. So for bytes
 * b0, b1, b2,..., b15, the state matrix will look like this:
 *
 * [b0, b4, b8 , b12]
 * [b1, b5, b9 , b13]
 * [b2, b6, b10, b14]
 * [b3, b7, b11, b15]
 * 
 * As such, we read/write the bytes from/to the files sequentially into a 
 * separate buffer, and fill in the state block from there. The cipher then
 * assumes that the state is arranged as above. 
 *
 * @param fdin - File descriptor for the plaintext file. Should be a binary file
 *               that has already been opened for reading.
 * @param fdout - File descriptor for the cipher file, which should be a new 
 *                binary file opened for writing.
 * @param key - Pointer to the encryption key.
 */
bool encrypt_file(FILE *fdin, FILE *fdout, aes_key_t *key) {
  int i, b, num_blocks;
  size_t bytes_read;
  long int file_size;
  uint8_t buffer[18]; /* A little bit of extra, just in case */
  uint8_t **state;
  
  /* Initialize state block on the heap */
  state = (uint8_t**)malloc(4 * sizeof(uint8_t*)); 
  for (i = 0; i < 4; i++)
	state[i] = (uint8_t*)malloc(4); /* Allocate each row */

  /* Determine the size of the file. */
  fseek(fdin, 0L, SEEK_END);
  file_size = ftell(fdin);
  rewind(fdin);

  /* Determine the number of blocks to encrypt */
  num_blocks = file_size / AES_BLOCK_SIZE;
  if (file_size % AES_BLOCK_SIZE) num_blocks++;

  VERBOSE("Size of file: %ld bytes (%d blocks)\n", file_size, num_blocks);

  /* Encrypt each block */
  
  for (b = 0; b < num_blocks; b++) {
	if (flags.verbose) {
	  printf("Encrypting block %d of %d.\r", b+1, num_blocks);
	  fflush(stdin);
	}

	/* Read in next 16 bytes. */
	bytes_read = fread(buffer, sizeof(uint8_t), 16, fdin);
	if (ferror(fdin) != 0) {
	  fprintf(stderr, PROGRAM_NAME ": Error: File read error.\n");
	  return false;
	}

	/* If we did not read a full block, pad with 0's */
	while (bytes_read < 16)
	  buffer[bytes_read++] = 0;
	
	/* Fill state block */
	for (i = 0; i < AES_BLOCK_SIZE; i++)
	  state[i%4][i/4] = buffer[i];
	
	aes_cipher(state, key); /* Run cipher on the current state matrix*/

	/* Get the newly encrypted byte sequence */
	for (i = 0; i < AES_BLOCK_SIZE; i++)
	  buffer[i] = state[i%4][i/4];
	
	/* Write result to the output file */
	if (fwrite(buffer, sizeof(uint8_t), 16, fdout) != 16) {
	  fprintf(stderr, PROGRAM_NAME ": Error: File write error.\n");
	  return false;
	}
  }
  VERBOSE("\n");

  for (i = 0; i < 4; i++)
	free(state[i]);
  free(state);
  
  return true;
}

/**
 * Cipher files are designated with OUPUT_EXTENSION (.aes)
 * This function takes the input file and creates a new
 * file path that indicates either the current directory or
 * the directory selected with -d option, and appends the
 * .aes extension. 
 */
char * create_out_file_name(char *in_path) {
  char *in_path_cp;
  char *out_name, *out_dir, *out_path;

  out_path = (char*)malloc(FILENAME_MAX);

  in_path_cp = strdup(in_path);
  out_name = basename(in_path_cp);

  if (flags.out_directory == NULL) {
	out_dir = "./";
  }
  else {
	out_dir = flags.out_directory;
  }

  strncpy(out_path, out_dir, FILENAME_MAX);
  strncat(out_path, out_name, FILENAME_MAX - strlen(out_dir));

  /* Append cipher output extension */
  strncat(out_path, CIPHER_EXTENSION, FILENAME_MAX - strlen(out_dir));

  free(in_path_cp);
  return out_path;
}

/**
 * Program main function.
 *
 * We leave the meat of the processing to the above functions. Here, we 
 * simply process the command line arguments.
 */
int
main(int argc, char *argv[])
{
  int opt;
  char *out_name;
  FILE *keyfd, *infd, *outfd;
  
  /* Parse command options */
  while ((opt = getopt_long(argc, argv, opts_str, long_opts, NULL)) != -1) {
	switch (opt) { 
	  //case 'f': flags.force = true; break;
	  
	  //case 'b': flags.burn_file = true; break;

	  /* Verbose option */
	case 'v': flags.verbose = true; break;

	  /* Key size option */
	case 's': flags.key_size = atoi(optarg);
	  if (flags.key_size != key_16_bytes
		  && flags.key_size != key_24_bytes
		  && flags.key_size != key_32_bytes) {
		exit_error(PROGRAM_NAME ": Error: Invalid key size specified.\n");
	  }
	  break;

	  /* Key file name option */
	case 'k': flags.key_file_name = optarg; break;

	  /* Specified output directory */
	case 'd': flags.out_directory = optarg; break;
		
	default:
	  exit_error(PROGRAM_NAME ": Error: Invalid option indicated.\n");
	  break;
	}
  }

  /* Generate Encryption Key */
  VERBOSE("Generating encryption key.\n");
  key_init(&encrypt_key, flags.key_size);

  /* Open file to save key */
  /* Use default file name if not specified by user */
  if (flags.key_file_name == NULL) {
	VERBOSE("Key file not specified. Creating file '" DEFAULT_KEY_FILE "'.\n");
	flags.key_file_name = DEFAULT_KEY_FILE;
  }

  /* Save key to file */
  VERBOSE("Saving encryption key to file '%s'.\n", flags.key_file_name);
  keyfd = fopen(flags.key_file_name, "w");
  if (keyfd == NULL) {
	exit_error(PROGRAM_NAME ": Error: Could not create encryption key" \
			   " file '%s'.\n",\
			   flags.key_file_name);
  }
  save_key(&encrypt_key, keyfd);
  fclose(keyfd); /* Close file */

  /* Encrypt specified files using newly generated key */

  if ((optind == argc) || (argv[optind][0] == '-')) {
	/* Encrypt standard input if no file specified. */
    VERBOSE("Reading from Standard Input.\n");
	infd = stdin;

	outfd = fopen(DEFAULT_OUT_FILE OUTPUT_EXTENSION, "wb"); 

	encrypt_file(infd, outfd, &encrypt_key);
	optind++;
  }

  for (; optind < argc; optind++) {
	infd = fopen(argv[optind], "rb"); /* Open file for reading */
	
	if (infd == NULL) {
	  fprintf(stderr, PROGRAM_NAME ": Error: Failed to open file '%s'.\n",
			  argv[optind]);
	}
	else {
	  out_name = create_out_file_name(argv[optind]);
	  outfd = fopen(out_name, "wb"); 

	  if (outfd == NULL) {
		fprintf(stderr, PROGRAM_NAME ": Error: Failed to create file '%s'.\n",
				out_name);
		fclose(infd);
	  }
	  else {
		VERBOSE("Encrypting file '%s'...\n", argv[optind]);
		if (encrypt_file(infd, outfd, &encrypt_key)) {
		  printf(PROGRAM_NAME ": Cipher '%s' created from file '%s'.\n",
				 out_name, argv[optind]);
		}
		fclose(infd);
		fclose(outfd);
	  }
	  free(out_name);
	}
  }

  printf(PROGRAM_NAME ": Encryption complete. Key stored at file '%s'.\n",
		 flags.key_file_name);
  
  exit(EXIT_SUCCESS);
}
