/*
 * Copyright (c) 1993-1997 by Alexander V. Lukyanov (lav@yars.free.net)
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

#ifndef KEYMAP_H
#define KEYMAP_H

enum  Action
{
// Movement actions
   CHAR_LEFT=1024,
   CHAR_RIGHT,
   WORD_LEFT,
   WORD_RIGHT,
   LINE_BEGIN,
   LINE_END,
   TEXT_BEGIN,
   TEXT_END,
   NEXT_PAGE,
   PREV_PAGE,
   PAGE_TOP,
   PAGE_BOTTOM,
   TO_LINE_NUMBER,
   TO_OFFSET,
   TO_PREVIOUS_LOC,
   LINE_UP,
   LINE_DOWN,

// Delete actions
   DELETE_CHAR,
   BACKSPACE_CHAR,
   DELETE_WORD,
   BACKWARD_DELETE_WORD,
   FORWARD_DELETE_WORD,
   DELETE_TO_EOL,
   DELETE_LINE,
   UNDELETE,

// Insert actions
   INDENT,
   UNINDENT,
   AUTOINDENT,
   NEWLINE,
   COPY_FROM_UP,
   COPY_FROM_DOWN,

// File ops
   LOAD_FILE,
   SWITCH_FILE,
   SAVE_FILE,
   SAVE_FILE_AS,
   FILE_INFO,

// Block ops
   COPY_BLOCK,
   MOVE_BLOCK,
   DELETE_BLOCK,
   SET_BLOCK_END,
   SET_BLOCK_BEGIN,
   READ_BLOCK,
   WRITE_BLOCK,
   PIPE_BLOCK,
   INDENT_BLOCK,
   UNINDENT_BLOCK,
   INSERT_PREFIX,
   TO_UPPER,
   TO_LOWER,
   EXCHANGE_CASE,
   BLOCK_HIDE,
   BLOCK_TYPE,
   BLOCK_FUNC_BAR,
   MARK_LINE,
   MARK_TO_EOL,

// Search
   SEARCH_FORWARD,
   SEARCH_BACKWARD,
   START_REPLACE,
   CONT_SEARCH,
   FIND_MATCH_BRACKET,
   FIND_BLOCK_BEGIN,
   FIND_BLOCK_END,

// Format
   FORMAT_ONE_PARA,
   FORMAT_DOCUMENT,
   CENTER_LINE,
   AJUST_RIGHT_LINE,
   FORMAT_FUNC_BAR,

// Others
   CALCULATOR,
   DRAW_FRAMES,
   TABS_EXPAND,
   TEXT_OPTIMIZE,
   CHOOSE_CHAR,
   UNIX_DOS_TRANSFORM,

// Options
   EDITOR_OPTIONS,
   TERMINAL_OPTIONS,
   FORMAT_OPTIONS,
   APPEARENCE_OPTIONS,
   PROGRAM_OPTIONS,
   COLOR_TUNING,
   SAVE_OPTIONS,
   SAVE_OPTIONS_LOCAL,

   ENTER_CONTROL_CHAR,
   ENTER_CHAR_CODE,

   WINDOW_RESIZE,

   EDITOR_HELP,
   CONTEXT_HELP,

   SUSPEND_EDITOR,
   QUIT_EDITOR,
#define  CANCEL   QUIT_EDITOR

   COMPILE_CMD,
   MAKE_CMD,
   RUN_CMD,
   SHELL_CMD,
   ONE_SHELL_CMD,

   COMMENT_LINE,
   REFRESH_SCREEN,

   ENTER_MENU,

   SWITCH_INSERT_MODE,
   SWITCH_HEX_MODE,
   SWITCH_AUTOINDENT_MODE,
   SWITCH_RUSSIAN_MODE,
   SWITCH_TEXT_MODE,
   SWITCH_GRAPH_MODE,

   MOUSE_ACTION,
   NO_ACTION
};

typedef  void  (*ActionProc)();

struct   ActionProcRec
{
   int         action;
   ActionProc  proc;
};

struct   ActionNameRec
{
   int   action;
   char  *name;
};

struct   ActionCodeRec
{
   int   action;
   char  *code;
};

extern   char  StringTyped[];
extern   int   StringTypedLen;
extern   ActionProcRec  EditorActionProcTable[];
extern   ActionCodeRec  *ActionCodeTable;
extern   ActionCodeRec  DefaultActionCodeTable[];

int   GetNextAction(void);
char  *GetActionString(int action);
void  ReadActionMap(FILE*);
void  WriteActionMap(FILE*);
ActionProc  GetActionProc(ActionProcRec*,int action);
void  EditorReadKeymap();
void  RebuildKeyTree();

#endif /* KEYMAP_H */