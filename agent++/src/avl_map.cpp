/*_############################################################################
  _## 
  _##  AGENT++ 4.0 - avl_map.cpp  
  _## 
  _##  Copyright (C) 2000-2013  Frank Fock and Jochen Katz (agentpp.com)
  _##  
  _##  Licensed under the Apache License, Version 2.0 (the "License");
  _##  you may not use this file except in compliance with the License.
  _##  You may obtain a copy of the License at
  _##  
  _##      http://www.apache.org/licenses/LICENSE-2.0
  _##  
  _##  Unless required by applicable law or agreed to in writing, software
  _##  distributed under the License is distributed on an "AS IS" BASIS,
  _##  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  _##  See the License for the specific language governing permissions and
  _##  limitations under the License.
  _##  
  _##########################################################################*/
// This may look like C code, but it is really -*- C++ -*-
/* 
Copyright (C) 1988 Free Software Foundation
    written by Doug Lea (dl@rocky.oswego.edu)

This file is part of the GNU C++ Library.  This library is free
software; you can redistribute it and/or modify it under the terms of
the GNU Library General Public License as published by the Free
Software Foundation; either version 2 of the License, or (at your
option) any later version.  This library is distributed in the hope
that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the GNU Library General Public License for more details.
You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free Software
Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/
#include <libagent.h>

#ifndef WIN32
//#include <stream.h>
#endif
#include <agent_pp/avl_map.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif


/*
 constants & inlines for maintaining balance & thread status in tree nodes
*/

#define AVLBALANCEMASK    3
#define AVLBALANCED       0
#define AVLLEFTHEAVY      1
#define AVLRIGHTHEAVY     2

#define LTHREADBIT        4
#define RTHREADBIT        8


static inline int bf(OidxPtrEntryPtrAVLNode* t)
{
  return t->stat & AVLBALANCEMASK;
}

static inline void set_bf(OidxPtrEntryPtrAVLNode* t, int b)
{
  t->stat = (t->stat & ~AVLBALANCEMASK) | (b & AVLBALANCEMASK);
}


static inline int rthread(OidxPtrEntryPtrAVLNode* t)
{
  return t->stat & RTHREADBIT;
}

static inline void set_rthread(OidxPtrEntryPtrAVLNode* t, int b)
{
  if (b)
    t->stat |= RTHREADBIT;
  else
    t->stat &= ~RTHREADBIT;
}

static inline int lthread(OidxPtrEntryPtrAVLNode* t)
{
  return t->stat & LTHREADBIT;
}

static inline void set_lthread(OidxPtrEntryPtrAVLNode* t, int b)
{
  if (b)
    t->stat |= LTHREADBIT;
  else
    t->stat &= ~LTHREADBIT;
}

/*
 traversal primitives
*/


OidxPtrEntryPtrAVLNode* OidxPtrEntryPtrAVLMap::leftmost() const
{
  OidxPtrEntryPtrAVLNode* t = root;
  if (t != 0) while (t->lt != 0) t = t->lt;
  return t;
}

OidxPtrEntryPtrAVLNode* OidxPtrEntryPtrAVLMap::rightmost() const
{
  OidxPtrEntryPtrAVLNode* t = root;
  if (t != 0) while (t->rt != 0) t = t->rt;
  return t;
}

OidxPtrEntryPtrAVLNode* OidxPtrEntryPtrAVLMap::succ(OidxPtrEntryPtrAVLNode* t) const
{
  OidxPtrEntryPtrAVLNode* r = t->rt;
  if (!rthread(t)) while (!lthread(r)) r = r->lt;
  return r;
}

OidxPtrEntryPtrAVLNode* OidxPtrEntryPtrAVLMap::pred(OidxPtrEntryPtrAVLNode* t) const
{
  OidxPtrEntryPtrAVLNode* l = t->lt;
  if (!lthread(t)) while (!rthread(l)) l = l->rt;
  return l;
}


Pix OidxPtrEntryPtrAVLMap::seek(OidxPtr  key) const
{
  OidxPtrEntryPtrAVLNode* t = root;
  if (t == 0)
    return 0;
  for (;;)
  {
    int cmp = OidxPtrCMP(key, t->item);
    if (cmp == 0)
      return Pix(t);
    else if (cmp < 0)
    {
      if (lthread(t))
        return 0;
      else
        t = t->lt;
    }
    else if (rthread(t))
      return 0;
    else
      t = t->rt;
  }
}

Pix OidxPtrEntryPtrAVLMap::seek_inexact(OidxPtr  key) const
{
  OidxPtrEntryPtrAVLNode* t = root;
  if (t == 0)
    return 0;
  for (;;)
  {
    int cmp = OidxPtrCMP(key, t->item);
    if (cmp == 0)
      return Pix(t);
    else if (cmp < 0)
    {
      if (lthread(t))
        return Pix(t);
      else
        t = t->lt;
    }
    else if (rthread(t))
      return Pix(t);
    else
      t = t->rt;
  }
}


/*
 The combination of threads and AVL bits make adding & deleting
 interesting, but very awkward.

 We use the following statics to avoid passing them around recursively
*/

static bool _need_rebalancing;   // to send back balance info from rec. calls
static OidxPtr*   _target_item;     // add/del_item target
static OidxPtrEntryPtrAVLNode* _found_node; // returned added/deleted node
static int    _already_found;   // for deletion subcases

void OidxPtrEntryPtrAVLMap:: _add(OidxPtrEntryPtrAVLNode*& t)
{
  int cmp = OidxPtrCMP(*_target_item, t->item);
  if (cmp == 0)
  {
    _found_node = t;
    return;
  }
  else if (cmp < 0)
  {
    if (lthread(t))
    {
      ++count;
      _found_node = new OidxPtrEntryPtrAVLNode(*_target_item, def);
      set_lthread(_found_node, 1);
      set_rthread(_found_node, 1);
      _found_node->lt = t->lt;
      _found_node->rt = t;
      t->lt = _found_node;
      set_lthread(t, 0);
      _need_rebalancing = true;
    }
    else
      _add(t->lt);
    if (_need_rebalancing)
    {
      switch(bf(t))
      {
      case AVLRIGHTHEAVY:
        set_bf(t, AVLBALANCED);
        _need_rebalancing = false;
        return;
      case AVLBALANCED:
        set_bf(t, AVLLEFTHEAVY);
        return;
      case AVLLEFTHEAVY:
	{
        OidxPtrEntryPtrAVLNode* l = t->lt;
        if (bf(l) == AVLLEFTHEAVY)
        {
          if (rthread(l))
            t->lt = l;
          else
            t->lt = l->rt;
          set_lthread(t, rthread(l));
          l->rt = t;
          set_rthread(l, 0);
          set_bf(t, AVLBALANCED);
          set_bf(l, AVLBALANCED);
          t = l;
          _need_rebalancing = false;
        }
        else
        {
          OidxPtrEntryPtrAVLNode* r = l->rt;
          set_rthread(l, lthread(r));
          if (lthread(r))
            l->rt = r;
          else
            l->rt = r->lt;
          r->lt = l;
          set_lthread(r, 0);
          set_lthread(t, rthread(r));
          if (rthread(r))
            t->lt = r;
          else
            t->lt = r->rt;
          r->rt = t;
          set_rthread(r, 0);
          if (bf(r) == AVLLEFTHEAVY)
            set_bf(t, AVLRIGHTHEAVY);
          else
            set_bf(t, AVLBALANCED);
          if (bf(r) == AVLRIGHTHEAVY)
            set_bf(l, AVLLEFTHEAVY);
          else
            set_bf(l, AVLBALANCED);
          set_bf(r, AVLBALANCED);
          t = r;
          _need_rebalancing = false;
          return;
        }
	}
      }
    }
  }
  else
  {
    if (rthread(t))
    {
      ++count;
      _found_node = new OidxPtrEntryPtrAVLNode(*_target_item, def);
      set_rthread(t, 0);
      set_lthread(_found_node, 1);
      set_rthread(_found_node, 1);
      _found_node->lt = t;
      _found_node->rt = t->rt;
      t->rt = _found_node;
      _need_rebalancing = true;
    }
    else
      _add(t->rt);
    if (_need_rebalancing)
    {
      switch(bf(t))
      {
      case AVLLEFTHEAVY:
        set_bf(t, AVLBALANCED);
        _need_rebalancing = false;
        return;
      case AVLBALANCED:
        set_bf(t, AVLRIGHTHEAVY);
        return;
      case AVLRIGHTHEAVY:
	{
        OidxPtrEntryPtrAVLNode* r = t->rt;
        if (bf(r) == AVLRIGHTHEAVY)
        {
          if (lthread(r))
            t->rt = r;
          else
            t->rt = r->lt;
          set_rthread(t, lthread(r));
          r->lt = t;
          set_lthread(r, 0);
          set_bf(t, AVLBALANCED);
          set_bf(r, AVLBALANCED);
          t = r;
          _need_rebalancing = false;
        }
        else
        {
          OidxPtrEntryPtrAVLNode* l = r->lt;
          set_lthread(r, rthread(l));
          if (rthread(l))
            r->lt = l;
          else
            r->lt = l->rt;
          l->rt = r;
          set_rthread(l, 0);
          set_rthread(t, lthread(l));
          if (lthread(l))
            t->rt = l;
          else
            t->rt = l->lt;
          l->lt = t;
          set_lthread(l, 0);
          if (bf(l) == AVLRIGHTHEAVY)
            set_bf(t, AVLLEFTHEAVY);
          else
            set_bf(t, AVLBALANCED);
          if (bf(l) == AVLLEFTHEAVY)
            set_bf(r, AVLRIGHTHEAVY);
          else
            set_bf(r, AVLBALANCED);
          set_bf(l, AVLBALANCED);
          t = l;
          _need_rebalancing = false;
          return;
        }
	}
      }
    }
  }
}

    
EntryPtr& OidxPtrEntryPtrAVLMap::operator [] (OidxPtr  item)
{
  if (root == 0)
  {
    ++count;
    root = new OidxPtrEntryPtrAVLNode(item, def);
    set_rthread(root, 1);
    set_lthread(root, 1);
    return root->cont;
  }
  else
  {
    _target_item = &item;
    _need_rebalancing = false;
    _add(root);
    return _found_node->cont;
  }
}


void OidxPtrEntryPtrAVLMap::_del(OidxPtrEntryPtrAVLNode* par, OidxPtrEntryPtrAVLNode*& t)
{
  int comp;
  if (_already_found)
  {
    if (rthread(t))
      comp = 0;
    else
      comp = 1;
  }
  else 
    comp = OidxPtrCMP(*_target_item, t->item);
  if (comp == 0)
  {
    if (lthread(t) && rthread(t))
    {
      _found_node = t;
      if (t == par->lt)
      {
        set_lthread(par, 1);
        par->lt = t->lt;
      }
      else
      {
        set_rthread(par, 1);
        par->rt = t->rt;
      }
      _need_rebalancing = true;
      return;
    }
    else if (lthread(t))
    {
      _found_node = t;
      OidxPtrEntryPtrAVLNode* s = succ(t);
      if (s != 0 && lthread(s))
        s->lt = t->lt;
      t = t->rt;
      _need_rebalancing = true;
      return;
    }
    else if (rthread(t))
    {
      _found_node = t;
      OidxPtrEntryPtrAVLNode* p = pred(t);
      if (p != 0 && rthread(p))
        p->rt = t->rt;
      t = t->lt;
      _need_rebalancing = true;
      return;
    }
    else                        // replace item & find someone deletable
    {
      OidxPtrEntryPtrAVLNode* p = pred(t);
      t->item = p->item;
      t->cont = p->cont;
      _already_found = 1;
      comp = -1;                // fall through below to left
    }
  }

  if (comp < 0)
  {
    if (lthread(t))
      return;
    _del(t, t->lt);
    if (!_need_rebalancing)
      return;
    switch (bf(t))
    {
    case AVLLEFTHEAVY:
      set_bf(t, AVLBALANCED);
      return;
    case AVLBALANCED:
      set_bf(t, AVLRIGHTHEAVY);
      _need_rebalancing = false;
      return;
    case AVLRIGHTHEAVY:
      {
      OidxPtrEntryPtrAVLNode* r = t->rt;
      switch (bf(r))
      {
      case AVLBALANCED:
        if (lthread(r))
          t->rt = r;
        else
          t->rt = r->lt;
        set_rthread(t, lthread(r));
        r->lt = t;
        set_lthread(r, 0);
        set_bf(t, AVLRIGHTHEAVY);
        set_bf(r, AVLLEFTHEAVY);
        _need_rebalancing = false;
        t = r;
        return;
      case AVLRIGHTHEAVY:
        if (lthread(r))
          t->rt = r;
        else
          t->rt = r->lt;
        set_rthread(t, lthread(r));
        r->lt = t;
        set_lthread(r, 0);
        set_bf(t, AVLBALANCED);
        set_bf(r, AVLBALANCED);
        t = r;
        return;
      case AVLLEFTHEAVY:
	{
        OidxPtrEntryPtrAVLNode* l = r->lt;
        set_lthread(r, rthread(l));
        if (rthread(l))
          r->lt = l;
        else
          r->lt = l->rt;
        l->rt = r;
        set_rthread(l, 0);
        set_rthread(t, lthread(l));
        if (lthread(l))
          t->rt = l;
        else
          t->rt = l->lt;
        l->lt = t;
        set_lthread(l, 0);
        if (bf(l) == AVLRIGHTHEAVY)
          set_bf(t, AVLLEFTHEAVY);
        else
          set_bf(t, AVLBALANCED);
        if (bf(l) == AVLLEFTHEAVY)
          set_bf(r, AVLRIGHTHEAVY);
        else
          set_bf(r, AVLBALANCED);
        set_bf(l, AVLBALANCED);
        t = l;
        return;
	}
      }
    }
    }
  }
  else
  {
    if (rthread(t))
      return;
    _del(t, t->rt);
    if (!_need_rebalancing)
      return;
    switch (bf(t))
    {
    case AVLRIGHTHEAVY:
      set_bf(t, AVLBALANCED);
      return;
    case AVLBALANCED:
      set_bf(t, AVLLEFTHEAVY);
      _need_rebalancing = false;
      return;
    case AVLLEFTHEAVY:
      {
      OidxPtrEntryPtrAVLNode* l = t->lt;
      switch (bf(l))
      {
      case AVLBALANCED:
        if (rthread(l))
          t->lt = l;
        else
          t->lt = l->rt;
        set_lthread(t, rthread(l));
        l->rt = t;
        set_rthread(l, 0);
        set_bf(t, AVLLEFTHEAVY);
        set_bf(l, AVLRIGHTHEAVY);
        _need_rebalancing = false;
        t = l;
        return;
      case AVLLEFTHEAVY:
        if (rthread(l))
          t->lt = l;
        else
          t->lt = l->rt;
        set_lthread(t, rthread(l));
        l->rt = t;
        set_rthread(l, 0);
        set_bf(t, AVLBALANCED);
        set_bf(l, AVLBALANCED);
        t = l;
        return;
      case AVLRIGHTHEAVY:
	{
        OidxPtrEntryPtrAVLNode* r = l->rt;
        set_rthread(l, lthread(r));
        if (lthread(r))
          l->rt = r;
        else
          l->rt = r->lt;
        r->lt = l;
        set_lthread(r, 0);
        set_lthread(t, rthread(r));
        if (rthread(r))
          t->lt = r;
        else
          t->lt = r->rt;
        r->rt = t;
        set_rthread(r, 0);
        if (bf(r) == AVLLEFTHEAVY)
          set_bf(t, AVLRIGHTHEAVY);
        else
          set_bf(t, AVLBALANCED);
        if (bf(r) == AVLRIGHTHEAVY)
          set_bf(l, AVLLEFTHEAVY);
        else
          set_bf(l, AVLBALANCED);
        set_bf(r, AVLBALANCED);
        t = r;
        return;
	}
      }
      }
    }
  }
}

        

void OidxPtrEntryPtrAVLMap::del(OidxPtr  item)
{
  if (root == 0) return;
  _need_rebalancing = false;
  _already_found = false;
  _found_node = 0;
  _target_item = &item;
  _del(root, root);
  if (_found_node)
  {
    delete(_found_node);
    if (--count == 0)
      root = 0;
  }
}

void OidxPtrEntryPtrAVLMap::_kill(OidxPtrEntryPtrAVLNode* t)
{
  if (t != 0)
  {
    if (!lthread(t)) _kill(t->lt);
    if (!rthread(t)) _kill(t->rt);
    delete t;
  }
}


OidxPtrEntryPtrAVLMap::OidxPtrEntryPtrAVLMap(OidxPtrEntryPtrAVLMap& b) :OidxPtrEntryPtrMap(b.def)
{
  root = 0;
  count = 0;
  for (Pix i = b.first(); i != 0; b.next(i)) 
    (*this)[b.key(i)] = b.contents(i);
}


int OidxPtrEntryPtrAVLMap::OK()
{
  int v = 1;
  if (root == 0) 
    v = count == 0;
  else
  {
    int n = 1;
    OidxPtrEntryPtrAVLNode* trail = leftmost();
    OidxPtrEntryPtrAVLNode* t = succ(trail);
    while (t != 0)
    {
      ++n;
      v &= OidxPtrCMP(trail->item, t->item) < 0;
      trail = t;
      t = succ(t);
    }
    v &= n == count;
  }
  if (!v) error("invariant failure");
  return v;
}

#ifdef AGENTPP_NAMESPACE
}
#endif
