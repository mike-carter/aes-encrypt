/** error.h
 * 
 * Provides elements for global error reporting.
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
 
#ifndef _ERROR_H_
#define _ERROR_H_
 
#include <stdbool.h>

typedef enum {
    NO_ERR = 0,
    
    FILE_READ_ERR,
    FILE_WRIT_ERR,
    FILE_ACCESS_ERR,
    
    OUT_OF_MEMORY_ERR,
    
    UNKNOWN_ERR
} errorcode_t;

/**
 * Indicates what kind of error occurred. Set to NO_ERR by default. 
 */
extern errorcode_t errorcode;

/**
 * Field that is set to a string containing a printable error message
 */
extern char* errmsg;



#endif /* _ERROR_H_ */