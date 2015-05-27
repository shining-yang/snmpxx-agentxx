/*_############################################################################
  _## 
  _##  AGENT++ 4.0 - tools.cpp  
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

#include <libagent.h>

#include <agent_pp/tools.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

/*--------------------------- class AgentTools --------------------------*/

/**
 * Return a new string that is the concatenation of two given strings.
 *
 * @param prefix - The prefix string.
 * @param suffix - The suffix string.
 * @return A new string.
 */ 
char* AgentTools::make_concatenation(const char* prefix, const char* suffix)
{
	char* retval = new char[strlen(prefix)+strlen(suffix)+1];
	strcpy(retval, prefix);
	strcat(retval, suffix);
	return retval;
}
	
/**
 * Return the file size of a given file.
 *
 * @return The file size.
 */
long AgentTools::file_size(FILE *stream)
{
	long oldpos, length;
  
	oldpos = ftell(stream);
	fseek(stream, 0, SEEK_END);
	length = ftell(stream);
	fseek(stream, oldpos, SEEK_SET);
	return length;
}

/*--------------------------- class Timer --------------------------*/

bool Timer::in_time()
{
	time_t now;
	time(&now);
	
	if (lifetime>0)
		return (now <= timestamp + lifetime);
	return TRUE;
}

int Timer::due_time()
{
	time_t now;
	time(&now);
	
	if (now <= timestamp + lifetime)
		return (timestamp + lifetime - now);
	return 0;
}

bool Timer::in_time(int frac)
{
	time_t now;
	time(&now);
	
	if (lifetime>0)
		return (now <= timestamp + lifetime/frac);
	return TRUE;
}

#ifdef AGENTPP_NAMESPACE
}
#endif
