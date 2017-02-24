/** word32.h
 *
 * Defines macros and functions for performing byte-wise manipulation 
 * of 32-bit values.
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

#ifndef _WORD32_H_
#define _WORD32_H_

#include <stdint.h>

/* Macros */

#define CREATE_WORD32(B0, B1, B2, B3) \
    (B0 | ((uint32_t)B1 << 8) | ((uint32_t)B1 << 16) | ((uint32_t)B1 << 24))

/**
 * GET_BYTE[0-3](uint32_t word) 
 * GET_BYTE(uint32_t word, int byte) 
 * 
 * These macro functions return the value of the specified byte section
 * of the word. (zero-indexed)
 */
#define GET_BYTE0(W32)      (W32 & 0xFF)
#define GET_BYTE1(W32)      ((W32 >> 8) & 0xFF)
#define GET_BYTE2(W32)      ((W32 >> 16) & 0xFF)
#define GET_BYTE3(W32)      ((W32 >> 24) & 0xFF)
#define GET_BYTE(W32, N)    ((W32 >> (N * 8)) & 0xFF)

/**
 * SET_BYTE[0-3](uint32_t word, int value) 
 * SET_BYTE(uint32_t value, int value, int byte) 
 * 
 * These macro functions return the 32-bit word with the specified byte section
 * set to 
 */
#define SET_BYTE0(W32, B)   ((W32 & 0xFFFFFF00) | B)
#define SET_BYTE1(W32, B)   ((W32 & 0xFFFF00FF) | (B << 8))
#define SET_BYTE2(W32, B)   ((W32 & 0xFF00FFFF) | (B << 16))
#define SET_BYTE3(W32, B)   ((W32 & 0x00FFFFFF) | (B << 24))
#define SET_BYTE(W32, B, N) ((W32 & ~((uint32_t)0xFF << (N * 8))) | (B << (N * 8)))



#define ROTR_BYTES(W32, N)  ROTR_BITS(W32, (N * 8))
#define ROTL_BYTES(W32, N)  ROTL_BITS(W32, (N * 8))

#define ADD_MOD32(X, Y)     ((uint32_t) (((uint64_t)X + Y) & 0xFFFFFFFF))

/* Functions */

#endif _WORD32_H_