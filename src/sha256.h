/** sha256.h
 *
 * Provides functions for generating an SHA 256 message digest.
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

#ifdef _SHA256_H_
#define _SHA256_H_
 
#include <stdint.h>

void sha256_start();

void sha256_add(uint8_t input[], size_t in_size);

void sha256_finish(uint8_t ouput[]);

#endif /* _SHA256_H_ */