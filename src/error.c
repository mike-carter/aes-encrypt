/** error.c
 * 
 * Source file for error.h
 *
 * Author: Michael Carter
 * Date: 2017-24-02
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

#include "messages.h"

/**
 * Indicates what kind of error occured 
 */
errorcode_t errorcode = NO_ERR;

char* errmsg = NULL;


void seterror(errorcode_t err) {
  errorcode = err;
  
  switch (err) {
  case NO_ERR: errmsg = NULL; break;
  
  case OUT_OF_MEMORY_ERR: errmsg = OUT_OF_MEMORY_ERR_MSG; break;
  
  default:
    errorcode = UNKNOWN_ERR;
    errmsg = UNKNOWN_ERR_MSG;
    break;
  }
}

void clearerror() {
  errorcode = NO_ERR;
  errmsg = NULL;
}

void fileerror(const char* fname, char mode) {
  char* msg;
  
  err_occured = true;
  
  if (mode == 'w') {
    msg = FILE_WRIT_ERR_MSG;
  }
  else if (mode == 'r') {
    msg = FILE_READ_ERR_MSG;
  }
  else {
    msg = FILE_ACCESS_ERR_MSG;
  }
  
  errmsg = malloc(strlen(msg) + strlen(fname));
  if (errmsg == NULL) {
    errorcode = OUT_OF_MEMORY_ERR;
    errmsg = OUT_OF_MEMORY_ERR_MSG;
    return;
  }
  
  sprintf(errmsg, msg, fname);
}