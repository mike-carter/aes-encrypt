/** aes-file.c
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
 
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define FILE_VERSION     0x02
#define CREATED_BY_TAG   "CREATED_BY"
#define CREATED_DATE_TAG "CREATED_DATE"
#define CREATED_TIME_TAG "CREATED_TIME"
#define FILE_TYPE        "FILE_TYPE"

#define CONTAINER_SIZE 128
#define SALT_SIZE 16

static int write_ext(FILE* fp, const char* id, const char* content) {
  uint8_t* buf, buf_ptr, str_ptr;
  size_t id_len = strlen(id);
  size_t cont_len = strlen(content);
  size_t ext_len = id_size + cont_len + 1; /* The +1 is for the '\0' char */
  
  buf_ptr = buf = malloc(ext_len);
  if (buf == NULL) {
    error("Ran out of memory.")
    return -1;
  }
  
  /* put the extension length at the front, big-endian order. */
  *(buf_ptr++) = (uint8_t)(ext_len >> 8);
  *(buf_ptr++) = (uint8_t)(ext_len & 0xFF);
  
  /* write the id, then a Null delimiter, then the content. */
  str_ptr = (uint8_t*) id;
  while (id_size--) {
    *(buf_ptr++) = *(str_ptr++);
  }
  *(buf_ptr++) = '\0';
  
  str_ptr = (uint8_t*) content;
  while (cont_size--) {
    *(buf_ptr++) = *(str_ptr++);
  }
  
  if (fwrite(buf, 1, ext_len, fp) != ext_len) {
    error();
  }
  
  free(buf);
  return 0;
}

FILE* new_aes_file(const char* fname, uint8_t key[]) {
  uint8_t salt[SALT_SIZE];
  uint8_t buffer[CONTAINER_SIZE + 2];
  size_t bytes_read;
  FILE* rand;
  FILE* fp; 
  
  int write_tag(FILE*, const char*, const char*);
  
  /* Open the file for writing */
  if ((fp = fopen(fname, "w")) == NULL) {
    errorf("Unable to create file '%s'", fname);
    return NULL;
  }
  
  buffer[0] = 'A';
  buffer[1] = 'E';
  buffer[2] = 'S';
  buffer[3] = FILE_VERSION;
  buffer[4] = 0;
  if (fwrite(buffer, 1, 5, fp) != 5) {
    error("Failure to write file");
    return NULL;
  }
  
  /* Generate a random salt string */
  if ((rand = fopen("/dev/urandom", "r")) == NULL) {
    error("Unable to access /dev/urandom");
    return NULL;
  }
  
  bytes_read = fread(salt, 1, INIT_VECTOR_SIZE, rand);
  
  if (bytes_read != INIT_VECTOR_SIZE) {
    error("Error occurred while accessing /dev/urandom");
    return NULL;
  }
  fclose(rand);
  
  /* Write extensions */
}

