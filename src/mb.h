/*
 * Copyright (c) 2003 by Alexander V. Lukyanov (lav@yars.free.net)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* $Id$ */

#ifndef MB_H
#define MB_H

#ifdef USE_MULTIBYTE_CHARS
#include <wchar.h>
#include <wctype.h>

extern bool mb_mode;
extern int  MBCharSize;
extern int  MBCharWidth;

bool MBCheckLeftAt(offs o);
bool MBCheckAt(offs o);
wchar_t WCharAt(offs o);
void InsertWChar(wchar_t ch);

static inline bool MBCheckRight() { return MBCheckAt(Offset()); }
static inline bool MBCheckLeft()  { return MBCheckLeftAt(Offset()); }
static inline int CharWidthAt(offs o) { MBCheckAt(o); return MBCharWidth; }
static inline int CharSizeAt(offs o)  { MBCheckAt(o); return MBCharSize;  }
static inline int CharSize()  { return CharSizeAt(Offset()); }
static inline int CharWidth() { return CharWidthAt(Offset()); }
static inline int WChar() { return WCharAt(Offset()); }

void mb_get_col(const char *buf,int pos,int *col,int len);
void mb_char_left(const char *buf,int *pos,int *col,int len);
void mb_char_right(const char *buf,int *pos,int *col,int len);
int  mb_get_pos_for_col(const char *buf,int width,int len);

#else
# define mb_mode	(false)
# define MBCheckLeft()	(false)
# define MBCheckLeftAt(o) (false)
# define MBCheckAt(o)	(false)
# define MBCharSize	(1)
# define MBCharWidth	(1)
# define MBCheckRight()	(false)
# define CharWidthAt(o)	(1)
# define CharSizeAt(o)	(1)
# define CharWidth()	(1)
# define CharSize()	(1)
# define WCharAt(o)	CharAt((o))
# define WChar()	Char()
# define InsertWChar(ch) InsertChar(ch)
# define mb_get_col(buf,pos,col,len)	*(col)=(pos)
# define mb_char_left(buf,pos,col,len)  *(col)=--(*pos)
# define mb_char_right(buf,pos,col,len) *(col)=++(*pos)
# define mb_get_pos_for_col(buf,width,len) (width)
# define mblen(s,l)	(1)
#endif

#endif//MB_H