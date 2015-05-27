/*_############################################################################
  _## 
  _##  AGENT++ 4.0 - avl_map.h  
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


#ifndef _OidxPtrEntryPtrAVLMap_h
#define _OidxPtrEntryPtrAVLMap_h 1

#include <agent_pp/map.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif


struct OidxPtrEntryPtrAVLNode
{
  OidxPtrEntryPtrAVLNode*      lt;
  OidxPtrEntryPtrAVLNode*      rt;
  OidxPtr             item;
  EntryPtr            cont;
  char                stat;
                      OidxPtrEntryPtrAVLNode(OidxPtr  h, EntryPtr  c, 
					     OidxPtrEntryPtrAVLNode* l=0,
					     OidxPtrEntryPtrAVLNode* r=0);
                      ~OidxPtrEntryPtrAVLNode() {};
};

inline OidxPtrEntryPtrAVLNode::OidxPtrEntryPtrAVLNode(OidxPtr  h, EntryPtr  c, 
						     OidxPtrEntryPtrAVLNode* l,
						     OidxPtrEntryPtrAVLNode* r)
     :lt(l), rt(r), item(h), cont(c), stat(0) {}

typedef OidxPtrEntryPtrAVLNode* OidxPtrEntryPtrAVLNodePtr;


class AGENTPP_DECL OidxPtrEntryPtrAVLMap : public OidxPtrEntryPtrMap
{
protected:
  OidxPtrEntryPtrAVLNode*   root;

  OidxPtrEntryPtrAVLNode*   leftmost() const;
  OidxPtrEntryPtrAVLNode*   rightmost() const;
  OidxPtrEntryPtrAVLNode*   pred(OidxPtrEntryPtrAVLNode* t) const;
  OidxPtrEntryPtrAVLNode*   succ(OidxPtrEntryPtrAVLNode* t) const;
  void            _kill(OidxPtrEntryPtrAVLNode* t);
  void            _add(OidxPtrEntryPtrAVLNode*& t);
  void            _del(OidxPtrEntryPtrAVLNode* p, OidxPtrEntryPtrAVLNode*& t);

public:
                OidxPtrEntryPtrAVLMap(EntryPtr deflt);
                OidxPtrEntryPtrAVLMap(OidxPtrEntryPtrAVLMap& a);
  inline		~OidxPtrEntryPtrAVLMap();

  EntryPtr&          operator [] (OidxPtr  key);

  void			del(OidxPtr  key);

  inline Pix            first() const;
  inline void           next(Pix& i) const;
  inline OidxPtr&       key(Pix i) const;
  inline EntryPtr&   contents(Pix i);

  Pix			seek(OidxPtr key) const;
  Pix			seek_inexact(OidxPtr key) const;
  inline int		contains(OidxPtr key_) const;

  inline void		clear(); 

  Pix			last() const;
  void			prev(Pix& i) const;

  int			OK();
};

inline OidxPtrEntryPtrAVLMap::~OidxPtrEntryPtrAVLMap()
{
  _kill(root);
}

inline OidxPtrEntryPtrAVLMap::OidxPtrEntryPtrAVLMap(EntryPtr deflt)
  : OidxPtrEntryPtrMap(deflt)
{
  root = 0;
}

inline Pix OidxPtrEntryPtrAVLMap::first() const
{
  return Pix(leftmost());
}

inline Pix OidxPtrEntryPtrAVLMap::last() const
{
  return Pix(rightmost());
}

inline void OidxPtrEntryPtrAVLMap::next(Pix& i) const
{
  if (i != 0) i = Pix(succ((OidxPtrEntryPtrAVLNode*)i));
}

inline void OidxPtrEntryPtrAVLMap::prev(Pix& i) const
{
  if (i != 0) i = Pix(pred((OidxPtrEntryPtrAVLNode*)i));
}

inline OidxPtr& OidxPtrEntryPtrAVLMap::key(Pix i) const
{
  if (i == 0) error("null Pix");
  return ((OidxPtrEntryPtrAVLNode*)i)->item;
}

inline EntryPtr& OidxPtrEntryPtrAVLMap::contents(Pix i)
{
  if (i == 0) error("null Pix");
  return ((OidxPtrEntryPtrAVLNode*)i)->cont;
}

inline void OidxPtrEntryPtrAVLMap::clear()
{
  _kill(root);
  count = 0;
  root = 0;
}

inline int OidxPtrEntryPtrAVLMap::contains(OidxPtr key_) const
{
  return seek(key_) != 0;
}
#ifdef AGENTPP_NAMESPACE
}
#endif
#endif
