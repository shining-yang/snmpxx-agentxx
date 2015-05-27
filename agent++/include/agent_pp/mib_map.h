/*_############################################################################
  _## 
  _##  AGENT++ 4.0 - mib_map.h  
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


#ifndef _OidxPtrMibEntryPtrMap_h
#define _OidxPtrMibEntryPtrMap_h 1

typedef void* Pix;

#include <agent_pp/oidx_defs.h>
#include <agent_pp/mib_entry.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif


class AGENTPP_DECL OidxPtrMibEntryPtrMap
{
protected:
  int                   count;
  MibEntryPtr           def;

public:
                        OidxPtrMibEntryPtrMap(MibEntryPtr  deflt)
			    : count(0), def(deflt) {};
  virtual              ~OidxPtrMibEntryPtrMap() {};

  int                   length() const   // current number of items
      { return count; };
  int                   empty() const
      { return (count == 0);};

  virtual int           contains(OidxPtr  key);      // is key mapped?

  virtual void          clear();                 // delete all items

  virtual MibEntryPtr&  operator [] (OidxPtr  key) = 0; // access contents by key

  virtual void          del(OidxPtr  key) = 0;       // delete entry

  virtual Pix           first() = 0;             // Pix of first item or 0
  virtual void          next(Pix& i) = 0;        // advance to next or 0
  virtual OidxPtr&      key(Pix i) = 0;          // access key at i
  virtual MibEntryPtr&  contents(Pix i) = 0;     // access contents at i

  virtual int           owns(Pix i);             // is i a valid Pix  ?
  virtual Pix           seek(OidxPtr  key);          // Pix of key

  MibEntryPtr&          dflt()                   // access default val
      { return def; };

  void                  error(const char* msg);
  virtual int           OK() = 0;                // rep invariant
};

#ifdef AGENTPP_NAMESPACE
}
#endif

#endif
