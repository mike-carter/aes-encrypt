/** messages.h
 * 
 * Contains the output messages and 'printf' templates for various languages
 * Currently we can only support languages that use UTF-8, ISO/IEC 8859 encoding
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

#ifndef _MESSAGES_H_
#define _MESSAGES_H_

/* Language options: */
// TODO: add more languages
#define ENGLISH 1


/* Current Language */
#define LANGUAGE ENGLISH


/* Default language */
#ifndef LANGUAGE
#    define LANGUAGE ENGLISH
#endif


/* Messages */
#if LANGUAGE == ENGLISH 

#define FILE_ACCESS_ERR_MSG "'%' access error"
#define FILE_READ_ERR_MSG "Failed to read '%s'"
#define FILE_WRIT_ERR_MSG "Failed to write '%s'"
#define OUT_OF_MEMORY_MSG "Out of memory"
#define UNKNOWN_ERR_MSG "Unknown error occured during execution"

#endif

#endif /* _MESSAGES_H_ */