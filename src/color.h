/*
 * Copyright (c) 1993-2017 by Alexander V. Lukyanov (lav@yars.free.net)
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

enum
{
   STATUS_LINE=0,
   NORMAL_TEXT,
   BLOCK_TEXT,
   ERROR_WIN,
   VERIFY_WIN,
   CURR_BUTTON,
   HELP_WIN,
   DIALOGUE_WIN,
   MENU_WIN,
   DISABLED_ITEM,
   SCROLL_BAR,
   SHADOWED,
   SYNTAX1,
   SYNTAX2,
   SYNTAX3,
   SYNTAX4,
   SYNTAX5,
   SYNTAX6,
   HIGHLIGHT,

   MAX_COLOR_NO
};

#define SYNTAX_COLORS 6

struct attr
{
   chtype n_attr;    // normal
   chtype so_attr;   // stand-out
};

struct color
{
   int	    no;
   chtype   attr;
   int	    fg,bg;
};

void  init_attrs();

extern attr attr_table[MAX_COLOR_NO];

static inline
const attr *find_attr(int no)
{
   return attr_table+no;
}

extern char color_descriptions[MAX_COLOR_NO*2][256];

void  ParseColors();
const color *FindColor(const color *pal,int no);
static inline color *FindColor(color *pal,int no) { return const_cast<color*>(FindColor(const_cast<const color*>(pal),no)); }
void  DescribeColors(const color *,const color *);
void  DumpDefaultColors(FILE *);

extern color color_pal[MAX_COLOR_NO+1];
extern color bw_pal[MAX_COLOR_NO+1];
extern const color default_color_pal[MAX_COLOR_NO+1];
extern const color default_bw_pal[MAX_COLOR_NO+1];

void le_start_color();

#define NO_COLOR (-1)
