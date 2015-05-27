/*_############################################################################
  _## 
  _##  AGENT++ 4.0 - mib_avl_map.h  
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


#ifndef _OidxPtrMibEntryPtrAVLMap_h
#define _OidxPtrMibEntryPtrAVLMap_h 1

#include <agent_pp/agent++.h>
#include <agent_pp/mib_map.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif


struct OidxPtrMibEntryPtrAVLNode
{
  OidxPtrMibEntryPtrAVLNode*      lt;
  OidxPtrMibEntryPtrAVLNode*      rt;
  OidxPtr             item;
  MibEntryPtr         cont;
  char                stat;
                      OidxPtrMibEntryPtrAVLNode(OidxPtr  h, MibEntryPtr  c, 
                                    OidxPtrMibEntryPtrAVLNode* l=0, OidxPtrMibEntryPtrAVLNode* r=0);
                      ~OidxPtrMibEntryPtrAVLNode() {};
};

inline OidxPtrMibEntryPtrAVLNode::OidxPtrMibEntryPtrAVLNode(OidxPtr  h, MibEntryPtr  c, 
                                    OidxPtrMibEntryPtrAVLNode* l, OidxPtrMibEntryPtrAVLNode* r)
     :lt(l), rt(r), item(h), cont(c), stat(0) {}

typedef OidxPtrMibEntryPtrAVLNode* OidxPtrMibEntryPtrAVLNodePtr;


class AGENTPP_DECL OidxPtrMibEntryPtrAVLMap : public OidxPtrMibEntryPtrMap
{
protected:
  OidxPtrMibEntryPtrAVLNode*   root;

  OidxPtrMibEntryPtrAVLNode*   leftmost();
  OidxPtrMibEntryPtrAVLNode*   rightmost();
  OidxPtrMibEntryPtrAVLNode*   pred(OidxPtrMibEntryPtrAVLNode* t);
  OidxPtrMibEntryPtrAVLNode*   succ(OidxPtrMibEntryPtrAVLNode* t);
  void            _kill(OidxPtrMibEntryPtrAVLNode* t);
  void            _add(OidxPtrMibEntryPtrAVLNode*& t);
  void            _del(OidxPtrMibEntryPtrAVLNode* p, OidxPtrMibEntryPtrAVLNode*& t);

public:
                OidxPtrMibEntryPtrAVLMap(MibEntryPtr deflt);
                OidxPtrMibEntryPtrAVLMap(OidxPtrMibEntryPtrAVLMap& a);
  inline		~OidxPtrMibEntryPtrAVLMap();

  MibEntryPtr&          operator [] (OidxPtr key);

  void			del(OidxPtr  key);

  inline Pix            first();
  inline void           next(Pix& i);
  inline OidxPtr&       key(Pix i);
  inline MibEntryPtr&   contents(Pix i);

  Pix			seek(OidxPtr key);
  Pix			seek_inexact(OidxPtr key);
  inline int		contains(OidxPtr key_);

  inline void		clear(); 

  Pix			last();
  void			prev(Pix& i);

  int			OK();
};

inline OidxPtrMibEntryPtrAVLMap::~OidxPtrMibEntryPtrAVLMap()
{
  _kill(root);
}

inline OidxPtrMibEntryPtrAVLMap::OidxPtrMibEntryPtrAVLMap(MibEntryPtr deflt) :OidxPtrMibEntryPtrMap(deflt)
{
  root = 0;
}

inline Pix OidxPtrMibEntryPtrAVLMap::first()
{
  return Pix(leftmost());
}

inline Pix OidxPtrMibEntryPtrAVLMap::last()
{
  return Pix(rightmost());
}

inline void OidxPtrMibEntryPtrAVLMap::next(Pix& i)
{
  if (i != 0) i = Pix(succ((OidxPtrMibEntryPtrAVLNode*)i));
}

inline void OidxPtrMibEntryPtrAVLMap::prev(Pix& i)
{
  if (i != 0) i = Pix(pred((OidxPtrMibEntryPtrAVLNode*)i));
}

inline OidxPtr& OidxPtrMibEntryPtrAVLMap::key(Pix i)
{
  if (i == 0) error("null Pix");
  return ((OidxPtrMibEntryPtrAVLNode*)i)->item;
}

inline MibEntryPtr& OidxPtrMibEntryPtrAVLMap::contents(Pix i)
{
  if (i == 0) error("null Pix");
  return ((OidxPtrMibEntryPtrAVLNode*)i)->cont;
}

inline void OidxPtrMibEntryPtrAVLMap::clear()
{
  _kill(root);
  count = 0;
  root = 0;
}

inline int OidxPtrMibEntryPtrAVLMap::contains(OidxPtr key_)
{
  return seek(key_) != 0;
}

#ifdef AGENTPP_NAMESPACE
}
#endif

#endif
