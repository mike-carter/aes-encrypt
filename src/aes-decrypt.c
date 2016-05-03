/* 'aes-decrypt' file encryption program for GNU Linux

   Author: Michael Carter
   Date: 2016-05-02

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
#define PROGRAM_NAME "aes-decrypt"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <stdint.h>
#include <libgen.h>
#include <string.h>
#include <getopt.h>

#include "aes.h"

/* Option flags */
struct Options {
  //bool force; /* -f flag: chmod files if necessary */
  //bool burn_file; /* -b flag: shred file after encrypting */
  bool verbose; /* -v flag: print progress */
  bool use_stdout;
  char * out_directory;
};

/* Program options */
const struct option long_opts[] = {
  //{"force", no_argument, NULL, 'f'},
  //{"burn-after-read", no_argument, NULL, 'b'},
  {"out_dir", required_argument, NULL, 'd'},
  {"terminal", no_argument, NULL, 't'},
  {"verbose", no_argument, NULL, 'v'},
  {0, 0, 0, 0}
};
const char opts_str[] = "vtd:";


/* Static variables: */

static struct Options flags; /* User Selected Options */

static aes_key_t ekey; /* Contains the decryption key */

/* Program Functions: */

/**
 * Loads the key from the specified file and expands it.
 */
void load_key(FILE *keyfd) {
  size_t enc_len, data_len;
  char * b64_str;
  uint8_t * data_str;

  b64_str = (char *)malloc((size_t)80);
  if (fgets(b64_str, 80, keyfd) != b64_str) {
	exit_error(PROGRAM_NAME ": Failed to read key from file.");
  }
  enc_len = strlen(b64_str);
  data_str = base64_decode(b64_str, enc_len, &data_len);

  ekey.size = data_len;
  memmove(ekey.block, data_str, ekey.size);

  key_expansion(&ekey);
  free(b64_str);
  free(data_str);
}


void sub_bytes_inv(uint8_t **state) {
  int i;
  for (i = 0; i < 4; i++)
	bytesub_decrypt(state[i], 4);
}

/**
 * ShiftRows function of AES algorithm
 * Rotates the bytes in each row of the state block a certain 
 * number of times.
 */
void shift_rows_inv(uint8_t **state) {
  uint8_t temp;
  int r, i, c;
  /* First row is not changed.  */
  /* Rows 2, 3, 4 are rotated 1, 2, and 3 spaces to the left, respectively.  */
  for (r = 1; r < 4; r++) {
	for (i = 0; i < r; i++) {
	  for (c = 3; c >= 1; c--) {
		temp = state[r][c];
		state[r][c] = state[r][c-1];
		state[r][c-1] = temp;
	  }
	}
  }
}

/**
 * MixColumns function of AES algorithm
 */
void mix_columns_inv(uint8_t **state) {
  int c;
  uint8_t temp[4];
  for (c = 0; c < 4; c++) {
	temp[0] = ff_multiply(0x0e, state[0][c])
	  ^ ff_multiply(0x0b, state[1][c])
	  ^ ff_multiply(0x0d, state[2][c])
	  ^ ff_multiply(0x09, state[3][c]);
	temp[1] = ff_multiply(0x09, state[0][c])
	  ^ ff_multiply(0x0e, state[1][c])
	  ^ ff_multiply(0x0b, state[2][c])
	  ^ ff_multiply(0x0d, state[3][c]);
	temp[2] = ff_multiply(0x0d, state[0][c])
	  ^ ff_multiply(0x09, state[1][c])
	  ^ ff_multiply(0x0e, state[2][c])
	  ^ ff_multiply(0x0b, state[3][c]);
	temp[3] = ff_multiply(0x0b, state[0][c])
	  ^ ff_multiply(0x0d, state[1][c])
	  ^ ff_multiply(0x09, state[2][c])
	  ^ ff_multiply(0x0e, state[3][c]);

	state[0][c] = temp[0];
	state[1][c] = temp[1];
	state[2][c] = temp[2];
	state[3][c] = temp[3];
  }
}

/**
 * AddRoundKey function of AES algorithm
 * XOR's the block state against 
 */
void add_round_key_inv(uint8_t **state, int round) {
  int r, c;
  int keyind = 16 * (round + 1);

  for (c = 3; c >= 0; c--) {
	for (r = 3; r >= 0; r--) {
	  state[r][c] = state[r][c] ^ ekey.exp_block[--keyind];
	}
  }
}

void aes_cipher_inv(uint8_t **state) {
  int round, num_rounds;

  num_rounds = (ekey.size / 4) + (AES_BLOCK_SIZE / 4) + 2;
  
  add_round_key_inv(state, num_rounds);

  for (round = (num_rounds-1); round >= 1; round--) {
	shift_rows_inv(state);
	sub_bytes_inv(state);
	add_round_key_inv(state, round);
	mix_columns_inv(state);
  }

  shift_rows_inv(state);
  sub_bytes_inv(state);
  add_round_key_inv(state, 0);
}

/**
 * Main Decryption algorithm.
 */
bool decrypt_file(FILE *fdin, FILE *fdout, aes_key_t *key) {
  int i, b, num_blocks;
  long int file_size;
  uint8_t buffer[18];
  uint8_t **state;
  
  /* Initialize state block */
  state = (uint8_t**)malloc(4 * sizeof(uint8_t*));
  for (i = 0; i < 4; i++)
	state[i] = (uint8_t*)malloc(4);

  /* Get size of the file. */
  fseek(fdin, 0L, SEEK_END);
  file_size = ftell(fdin);
  rewind(fdin);

  /* Determine the number of blocks to encrypt */
  num_blocks = file_size / AES_BLOCK_SIZE;
  if (file_size % AES_BLOCK_SIZE) num_blocks++;

  VERBOSE("Size of file: %ld bytes (%d blocks)\n", file_size, num_blocks);
  
  /* Decrypt each block */
  
  for (b = 0; b < num_blocks; b++) {
	if (flags.verbose) {
	  printf("Decrypting block %d of %d.\r", b+1, num_blocks);
	  fflush(stdin);
	}

	/* Read in next 16 bytes. */
	fread(buffer, sizeof(uint8_t), 16, fdin);
	if (ferror(fdin) != 0) {
	  fprintf(stderr, PROGRAM_NAME ": Error: File read error.\n");
	  return false;
	}

	/* Fill state block */
	for (i = 0; i < AES_BLOCK_SIZE; i++)
	  state[i%4][i/4] = buffer[i];
	
	aes_cipher_inv(state); /* Run decryption */
	
    /* Get the newly decrypted byte sequence */
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
 * As with the encryption program, we generate an output file name based on the
 * input file. Usually a cipher will have the extension ".aes", and so the 
 * original file name will just be the cipher file minus the .aes extension.
 *
 * If the cipher file does not have the .aes extension, we will still try to
 * decode it, but instead, the output file will have a .txt extension appended
 * to the name.
 */
char * create_out_file_name(char *in_path) {
  char *out_path, *out_name, *out_dir;
  char *in_path_cp;
  char *ext_loc;

  out_path = (char*)malloc(FILENAME_MAX);

  in_path_cp = strdup(in_path);
  out_name = basename(in_path_cp);

  out_dir = flags.out_directory == NULL ? "./" : flags.out_directory;

  strncpy(out_path, out_dir, FILENAME_MAX);

  ext_loc = strstr(out_name, CIPHER_EXTENSION);
  if (ext_loc == NULL) {
	strncat(out_path, out_name, FILENAME_MAX - strlen(out_path));
	strncat(out_path, ".txt", FILENAME_MAX - strlen(out_path));
  }
  else {
	*ext_loc = '\0';
	strncat(out_path, out_name, FILENAME_MAX - strlen(out_path));
  }

  free(in_path_cp);
  return out_path;
}

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
	  
	case 't': flags.use_stdout = true; break;

	case 'd': flags.out_directory = optarg; break;
		  
	case 'v': flags.verbose = true; break;
		
	default:
	  exit_error(PROGRAM_NAME ": Error: Invalid option indicated.\n");
	  break;
	}
  }

  /* Load decryption key from provided file. 
	 (First argument must be the key file)
  */
  if (optind == argc) {
	exit_error(PROGRAM_NAME ": Error: Key file not specified.\n");
  }
  else if (argv[optind][0] == '-') {
	optind++;
	if (optind == argc) {
	  exit_error(PROGRAM_NAME ": Error: Key file not specified.\n");
	}
  }

  VERBOSE("Reading symmetric key from file '%s'\n", argv[optind]);
  keyfd = fopen(argv[optind], "r"); // Open File
  
  if (keyfd == NULL) {
	exit_error(PROGRAM_NAME ": Error attempting to read key file '%s'.\n",\
			argv[optind]);
  }
  load_key(keyfd); // Load and expand key
  fclose(keyfd); // Close key file
  optind++;

  /* Decrypt Cipher Files */
  for (; optind < argc; optind++) {
	infd = fopen(argv[optind], "rb"); /* Open file for reading */

	if (infd == NULL) {
	  fprintf(stderr, PROGRAM_NAME ": Error: Failed to open file '%s'.\n",
			  argv[optind]);
	}
	else {
	  if (flags.use_stdout) {
		outfd = stdout;
	  }
	  else {
		out_name = create_out_file_name(argv[optind]);
		outfd = fopen(out_name, "wb");
	  }

	  if (outfd == NULL) {
		fprintf(stderr, PROGRAM_NAME ": Error: Failed to create file '%s'.\n",
				out_name);
		fclose(infd);
	  }
	  else {
		VERBOSE("Decrypting file '%s'...\n", argv[optind]);
		if (decrypt_file(infd, outfd, &ekey)) {
		  printf(PROGRAM_NAME ": Plaintext file '%s' created from file '%s'.\n",
				 out_name, argv[optind]);
		}
		fclose(infd);
		if (!flags.use_stdout) fclose(outfd);
	  }
	  if (!flags.use_stdout) free(out_name);
	}
  }

  printf(PROGRAM_NAME ": Decryption complete.\n");
  exit(EXIT_SUCCESS);
}
