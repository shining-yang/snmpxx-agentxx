/*_############################################################################
  _## 
  _##  AGENT++ 4.0 - map.h  
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

#include <agent_pp/agent++.h>
#include <agent_pp/oidx_defs.h>
#include <agent_pp/entry.h>

#ifndef _OidxPtrEntryPtrMap_h
#define _OidxPtrEntryPtrMap_h 1

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

typedef void* Pix;



class AGENTPP_DECL OidxPtrEntryPtrMap
{
protected:
  int                   count;
  EntryPtr              def;

public:
                        OidxPtrEntryPtrMap(EntryPtr deflt)
			    : count(0), def(deflt) {};
  virtual              ~OidxPtrEntryPtrMap() {};

  int                   length() const { return count; }; // current number of items
  bool                  empty() const { return (count == 0);};

  virtual int           contains(OidxPtr key) const;      // is key mapped?

  virtual void          clear();                 // delete all items

  virtual EntryPtr&     operator [] (OidxPtr key) = 0; // access contents by key

  virtual void          del(OidxPtr key) = 0;    // delete entry

  virtual Pix           first() const = 0;       // Pix of first item or 0
  virtual void          next(Pix& i) const = 0;  // advance to next or 0
  virtual OidxPtr&      key(Pix i) const = 0;    // access key at i
  virtual EntryPtr&     contents(Pix i) = 0;     // access contents at i

  virtual int           owns(Pix i);             // is i a valid Pix  ?
  virtual Pix           seek(OidxPtr key) const; // Pix of key

  EntryPtr&             dflt() { return def; };  // access default val

  void                  error(const char* msg) const;
  virtual int           OK() = 0;                // rep invariant
};

#ifdef AGENTPP_NAMESPACE
}
#endif
#endif
