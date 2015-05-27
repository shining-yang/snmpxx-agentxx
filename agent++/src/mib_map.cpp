/*_############################################################################
  _## 
  _##  AGENT++ 4.0 - mib_map.cpp  
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

#ifdef __GNUG__
#pragma implementation
#endif
#include <agent_pp/mib_map.h>
#include <snmp_pp/log.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

static const char *loggerModuleName = "agent++.mib_map";

Pix OidxPtrMibEntryPtrMap::seek(OidxPtr  item)
{
  Pix i;
  for (i = first(); i != 0 && !(OidxPtrEQ(key(i), item)); next(i));
  return i;
}

int OidxPtrMibEntryPtrMap::owns(Pix idx)
{
  if (idx == 0) return 0;
  for (Pix i = first(); i; next(i)) if (i == idx) return 1;
  return 0;
}

void OidxPtrMibEntryPtrMap::clear()
{
  Pix i = first(); 
  while (i != 0)
  {
    del(key(i));
    i = first();
  }
}

int OidxPtrMibEntryPtrMap::contains (OidxPtr  item)
{
  return seek(item) != 0;
}


void OidxPtrMibEntryPtrMap::error(const char* msg)
{
  LOG_BEGIN(loggerModuleName, ERROR_LOG | 1);
  LOG("AVLMap");
  LOG(msg);
  LOG_END;
}

#ifdef AGENTPP_NAMESPACE
}
#endif
