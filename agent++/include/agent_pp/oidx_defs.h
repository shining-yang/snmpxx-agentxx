/*_############################################################################
  _## 
  _##  AGENT++ 4.0 - oidx_defs.h  
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


#ifndef _OidxPtrdefs_h
#define _OidxPtrdefs_h 1

#include <agent_pp/oidx_ptr.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif


// equality operator
#ifndef OidxPtrEQ
#define OidxPtrEQ(a, b)  ((*a) == (*b))
#endif

// less-than-or-equal
#ifndef OidxPtrLE
#define OidxPtrLE(a, b)  ((*a) <= (*b))
#endif

// comparison : less-than -> < 0; equal -> 0; greater-than -> > 0
#ifndef OidxPtrCMP
#define OidxPtrCMP(a, b) ( ((*a) <= (*b))? (((*a) == (*b))? 0 : -1) : 1 )
#endif

// hash function
#ifndef OidxPtrHASH
extern unsigned int hash(OidxPtr );
#define OidxPtrHASH(x)  hash(x)
#endif

// initial capacity for structures requiring one

#ifndef DEFAULT_INITIAL_CAPACITY
#define DEFAULT_INITIAL_CAPACITY 100
#endif

// HASHTABLE_TOO_CROWDED(COUNT, SIZE) is true iff a hash table with COUNT
// elements and SIZE slots is too full, and should be resized.
// This is so if available space is less than 1/8.

#define HASHTABLE_TOO_CROWDED(COUNT, SIZE) ((SIZE) - ((SIZE) >> 3) <= (COUNT))

#ifdef AGENTPP_NAMESPACE
}
#endif

#endif
