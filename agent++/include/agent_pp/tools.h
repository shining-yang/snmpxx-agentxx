/*_############################################################################
  _## 
  _##  AGENT++ 4.0 - tools.h  
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


#ifndef tools_h_
#define tools_h_

#include <sys/types.h>
#include <time.h>
#include <stdio.h>

#include <agent_pp/agent++.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif


class AGENTPP_DECL AgentTools {
 public:
	/**
	 * Return a new string that is the concatenation of two given strings.
	 *
	 * @param prefix - The prefix string.
	 * @param suffix - The suffix string.
	 * @return A new string.
	 */ 
	static char*	       	make_concatenation(const char*, const char*);

	/**
	 * Return the file size of a given file.
	 *
	 * @return The file size.
	 */
	static long		file_size(FILE*);
};

///////////////////////////////////////////////////////////////////////////////
// class Timer
//


class AGENTPP_DECL Timer {
public:
	Timer():	 timestamp(0), lifetime(0) { }
	Timer(int life): timestamp(0), lifetime(life) { }

	int		get_life()		   { return lifetime; }
	void		set_life(unsigned int sec) { lifetime = sec; }	

	bool		in_time();
	int		due_time();
	bool		in_time(int frac);
	void		set_timestamp()		{ time(&timestamp); }
	time_t		get_timestamp()		{ return timestamp; }

protected:
	time_t		timestamp;
	int		lifetime;
};	

#ifdef AGENTPP_NAMESPACE
}
#endif


#endif
