/*
 * Copyright (c) 2004 by Alexander V. Lukyanov (lav@yars.free.net)
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

#include <config.h>
#include "undo.h"

Undo undo;

Undo::Undo()
{
   chain_head=0;
   chain_ptr=0;
   chain_tail=0;
   group_open=0;
   current_group=0;
   enabled=true;
   locked=false;

   glue_changes=true;
   min_groups=4;
   max_size=0x10000000;
}
void Undo::Clear()
{
   while(chain_head)
   {
      Change *to_delete=chain_head;
      chain_head=chain_head->next;
      delete to_delete;
   }
   chain_tail=chain_ptr=0;
   current_group=0;
   group_open=0;
}
Undo::~Undo()
{
   Clear();
}

void Undo::BeginUndoGroup()
{
   if(!group_open)
      current_group++;
   group_open++;
   group_pos=CurrentPos;
   group_stdcol=stdcol;
}
void Undo::AddChange(Change *c)
{
   if(!Enabled())
   {
      delete c;
      return;
   }
   // cut undo list at current position, so redo is not possible.
   while(chain_ptr)
   {
      if(chain_ptr==chain_tail)
	 chain_tail=chain_ptr->prev;
      if(chain_ptr==chain_head)
	 chain_head=chain_ptr->next;
      if(chain_ptr->next)
	 chain_ptr->next->prev=chain_ptr->prev;
      if(chain_ptr->prev)
	 chain_ptr->prev->next=chain_ptr->next;
      Change *to_delete=chain_ptr;
      chain_ptr=chain_ptr->next;
      delete to_delete;
   }
   if(!group_open)
   {
      current_group++;
      group_pos=CurrentPos;
      group_stdcol=stdcol;
   }
   c->group=current_group;
   c->group_pos=group_pos;
   c->group_stdcol=group_stdcol;
   if(glue_changes && chain_tail && chain_tail->Join(c))
   {
      delete c;
      return;
   }
   c->prev=chain_tail;
   c->next=0;
   if(chain_tail)
      chain_tail->next=c;
   chain_tail=c;
   if(!chain_head)
      chain_head=c;
   group_pos=-1;
   group_stdcol=-1;
}
void Undo::EndUndoGroup()
{
   if(group_open<=0)
      return;
   group_open--;
   CheckSize();
}

Undo::Change::Change(type_t t,const char *l,num ls,const char *r,num rs)
{
   left=(char*)malloc(ls);
   if(!left)
      return;
   right=(char*)malloc(rs);
   if(!right)
   {
      free(left);
      left=0;
      return;
   }
   type=t;
   left_size=ls;
   right_size=rs;
   memcpy(left,l,ls);
   memcpy(right,r,rs);

   pos=CurrentPos;
   old_modified=modified;

   next=prev=0;
}

void Undo::UndoGroup()
{
   if(!chain_tail)
      return;
   if(!chain_ptr)
      chain_ptr=chain_tail;
   else
   {
      if(!chain_ptr->prev)
	 return;
      chain_ptr=chain_ptr->prev;
   }
   unsigned g=chain_ptr->group;
   locked=true;
   for(;;)
   {
      chain_ptr->Undo();
      if(chain_ptr->prev==0 || chain_ptr->prev->group!=g)
	 break;
      chain_ptr=chain_ptr->prev;
   }
   CurrentPos=chain_ptr->group_pos;
   stdcol=chain_ptr->group_stdcol;
   locked=false;
}
void Undo::RedoGroup()
{
   if(!chain_ptr)
      return;
   unsigned g=chain_ptr->group;
   locked=true;
   for(;;)
   {
      chain_ptr->Redo();
      chain_ptr=chain_ptr->next;
      if(!chain_ptr || chain_ptr->group!=g)
	 break;
   }
   locked=false;
}
void Undo::UndoOne()
{
   if(!chain_tail)
      return;
   if(!chain_ptr)
      chain_ptr=chain_tail;
   else
   {
      if(!chain_ptr->prev)
	 return;
      chain_ptr=chain_ptr->prev;
   }
   locked=true;
   chain_ptr->Undo();
   if(chain_ptr->group_pos!=-1)
   {
      CurrentPos=chain_ptr->group_pos;
      stdcol=chain_ptr->group_stdcol;
   }
   else
      stdcol=GetCol();
   locked=false;
}
void Undo::RedoOne()
{
   if(!chain_ptr)
      return;
   locked=true;
   chain_ptr->Redo();
   chain_ptr=chain_ptr->next;
   locked=false;
}

void Undo::Change::Undo()
{
   switch(type)
   {
   case DELETE:
      CurrentPos=pos-left_size;
      InsertBlock(left,left_size,right,right_size);
      break;
   case INSERT:
      CurrentPos=pos+left_size;
      DeleteBlock(left_size,right_size);
      break;
   case REPLACE:
      CurrentPos=pos;
      ReplaceBlock(left,left_size);
      break;
   }
   modified=old_modified;
}
void Undo::Change::Redo()
{
   CurrentPos=pos;
   switch(type)
   {
   case DELETE:
      DeleteBlock(left_size,right_size);
      break;
   case INSERT:
      InsertBlock(left,left_size,right,right_size);
      break;
   case REPLACE:
      ReplaceBlock(right,right_size);
      CurrentPos+=right_size;
      break;
   }
}
static bool mappend(char **buf,num *size,const char *add,num add_size)
{
   char *newbuf=(char*)realloc(*buf,*size+add_size);
   if(!newbuf)
      return false;
   *buf=newbuf;
   memmove(*buf+*size,add,add_size);
   *size+=add_size;
   return true;
}
static bool mprepend(char **buf,num *size,const char *add,num add_size)
{
   char *newbuf=(char*)realloc(*buf,*size+add_size);
   if(!newbuf)
      return false;
   *buf=newbuf;
   memmove(*buf+add_size,*buf,*size);
   memmove(*buf,add,add_size);
   *size+=add_size;
   return true;
}
bool Undo::Change::Join(const Change *c)
{
   if(c->type!=type)
      return false;
   if(c->group_pos!=-1 && c->group_pos!=c->pos)
      return false;
   switch(type)
   {
   case DELETE:
      if(pos-left_size!=c->pos)
	 return false;
      if(!mappend(&right,&right_size,c->right,c->right_size))
	 return false;
      if(!mprepend(&left,&left_size,c->left,c->left_size))
      {
	 right_size-=c->right_size;
	 return false;
      }
      break;
   case INSERT:
      if(pos+left_size!=c->pos)
	 return false;
      if(!mappend(&left,&left_size,c->left,c->left_size))
	 return false;
      if(!mprepend(&right,&right_size,c->right,c->right_size))
      {
	 left_size-=c->left_size;
	 return false;
      }
      break;
   case REPLACE:
      if(pos+right_size!=c->pos)
	 return false;
      if(!mappend(&right,&right_size,c->right,c->right_size))
	 return false;
      if(!mappend(&left,&left_size,c->left,c->left_size))
      {
	 right_size-=c->right_size;
	 return false;
      }
      break;
   }
   return true;
}

void Undo::CheckSize()
{
   Change *scan=0;
   if(chain_ptr)
      scan=chain_ptr->prev;
   else
      scan=chain_tail;
   if(!scan)
      return;
   unsigned g=scan->group;
   int count=1;
   num size=0;
   while(scan)
   {
      size+=scan->GetSize();
      scan=scan->prev;
      if(!scan)
	 break;
      if(scan->group!=g)
      {
	 g=scan->group;
	 count++;
	 if(size>=max_size && count>min_groups)
	 {
	    // cut the undo list.
	    scan->next->prev=0;
	    chain_head=scan->next;
	    while(scan)
	    {
	       Change *to_delete=scan;
	       scan=scan->prev;
	       delete to_delete;
	    }
	    break;
	 }
      }
   }
}