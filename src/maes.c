/** maes.c
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

/* Includes */ 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <time.h>
#include <getopt.h>
#include <stdint.h>
#include <sys/stat.h>

#define OPTIONS "vfbrdp:o:"
#define USAGE "maes -[vfb] [-p PASSWD] [-o OUT-FILE] [-d [-r] DIR | FILE]"

int verbose;



void main(int argc, char argv[][]) {
  struct option_flags flags;
  char passwd[]
  char* infname[FILENAME_MAX];
  char* outfname[FILENAME_MAX];
  
  
  
  
  exit(EXIT_SUCCESS);
}