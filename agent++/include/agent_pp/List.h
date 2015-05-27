/*_############################################################################
  _## 
  _##  AGENT++ 4.0 - List.h  
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

#ifndef _List_h_
#define _List_h_

#ifndef NULL
#define NULL 0
#endif

#include <agent_pp/agent++.h>
#include <agent_pp/avl_map.h>

#ifdef AGENTPP_NAMESPACE
namespace Agentpp {
#endif

template <class T> class List;
template <class T> class ListCursor;
template <class T> class OrderedList;
template <class T> class OidListCursor;

/**
 * The List collection class implements a double linked list template.
 *
 * @author Frank Fock
 * @version 3.4.4
 */

template <class T> class ListItem {
friend class List<T>;
friend class ListCursor<T>;
friend class OrderedList<T>;
public:
	/**
	 * Get the payload of the ListItem.
	 *
	 * @return
	 *    a pointer to T.
	 */
	T* getItem() { return item; }

private:
	ListItem<T> *prev, *next;
	T* item;
	ListItem(ListItem<T> *p, ListItem<T> *n, T* i):
                  prev(p), next(n), item(i) { }
};

template <class T> class List {
friend class ListCursor<T>;
public:
	/**
	 * Add an element at the list's beginning.
	 *
	 * @param t
	 *    a pointer to an instance to be added.
	 * @return 
	 *    the same pointer t
	 */ 
	T* addFirst(T* t) { 
		head = new ListItem<T>(NULL, head, t);
		if (head->next) head->next->prev = head;
		if (!tail)
			tail = head;
		return head->item;
	}

	/**
	 * Add an element at the list's end.
	 *
	 * @param t
	 *    a pointer to an instance to be added.
	 * @return 
	 *    the same pointer t
	 */ 
        T* addLast(T* t) {
		if (!head)
			return addFirst(t);
		else {
			ListItem<T> *p = tail;
		        p->next = new ListItem<T>(p, NULL, t);
			tail = p->next;
			return tail->item;
		}
	}

	/**
	 * Add an element at the list's end.
	 *
	 * @param t
	 *    a pointer to an instance to be added.
	 * @return 
	 *    the same pointer t
	 */ 
	T* add(T* t)	{ return addLast(t); }

	/**
	 * Insert an element before a given element.
	 *
	 * @param newElement
	 *    the element to add.
	 * @param element
	 *    a pointer to an element of the list. If the receiver does
	 *    not contain such an element, then item is appended to the 
	 *    list.
	 */
        T* insertBefore(T* item, T* elem) {
		if ((!head) || (head->item == elem)) {
			return addFirst(item);
		}
		else {
			ListItem<T>* tmp;
			ListItem<T>* h;
			tmp = head;
			while ((tmp->next) && (elem != tmp->next->item))
			      tmp = tmp->next;
			h = tmp->next;
			if (h) {
				tmp->next = new ListItem<T>(tmp, h, item);
				h->prev = tmp->next;
			}
			else {
				tmp->next = new ListItem<T>(tmp, NULL, item);
				tail = tmp->next;
			}
			return tmp->next->item;
		}
	}

	/**
	 * Insert an element after a given element.
	 *
	 * @param newElement
	 *    the element to add.
	 * @param element
	 *    a pointer to an element of the list. If the receiver does
	 *    not contain such an element, then item is appended to the 
	 *    list.
	 */
        T* insertAfter(T* item, T* elem) {
		if ((!tail) || (tail->item == elem)) {
			return addLast(item);
		}
		else {
			ListItem<T>* tmp;
			ListItem<T>* h;
			tmp = tail;
			while ((tmp->prev) && (elem != tmp->prev->item))
			      tmp = tmp->prev;
			h = tmp->prev;
			if (h) {
				tmp->prev = new ListItem<T>(h, tmp, item);
				h->next = tmp->prev;
			}
			else {
				tmp->prev = new ListItem<T>(NULL, tmp, item);
				head = tmp->prev;
			}
			return tmp->prev->item;
		}
	}

	/**
	 * Remove the first element.
	 *
	 * @return 
	 *    a pointer to the removed element or 0 if the list is empty.
	 */
	T* removeFirst() {
		if (head) {
			T* retval = head->item;
			ListItem<T> *temp = head;
			head		= head->next;
			if (!head) 
				tail = 0;
			else 
				head->prev	= NULL;
			delete temp;
			return retval;
		}
		return 0;
	}

	/**
	 * Remove the last element.
	 *
	 * @return 
	 *    a pointer to the removed item or 0 if the list is empty.
	 */
	T* removeLast() {
		if (tail) {
			T* retval = tail->item;
			ListItem<T> *temp = tail;
			tail		= tail->prev;
			if (!tail) 
				head = 0;
			else 
				tail->next	= NULL;
			delete temp;
			return retval;
		}
		return 0;
	}

	/**
	 * Remove and delete all elements from the receiver.
	 */
	void clearAll() {
		ListItem<T>* tmp = head;
		ListItem<T>* del;
		while (tmp) {
			delete tmp->item;
			del = tmp;
			tmp = tmp->next;
			delete del;
		}
		head = 0;
		tail = 0;
	}

	/**
	 * Empty the receiver list without freeing the memory of the
	 * list elements.
	 */
	void clear() {
		ListItem<T>* tmp = head;
		ListItem<T>* del;
		while (tmp) {
			del = tmp;
			tmp = tmp->next;
			delete del;
		}
		head = 0;
		tail = 0;
	}

	/**
	 * Remove an element.
	 * 
	 * @param item
	 *    a pointer to a list item.
	 * @return
	 *    the pointer to the given item, or 0 if the receiver does not
	 *    contain item.
	 */
	T* remove(T* i) {

		ListItem<T> *tmp;

		if (!head) 
			return 0;
		tmp = head;
		do {
			if (tmp->item == i) {
			        return remove(tmp);
			}
		}
		while ((tmp = tmp->next) != 0);
		return 0;
	}

	/**
	 * Remove an element using a cursor.
	 * Note: The cursor is invalid after this operation!
	 * 
	 * @param item
	 *    a pointer to a list cursor.
	 * @return
	 *    the pointer to the removed item.
	 */
	T* remove(ListItem<T>* victim) {
		T* i = victim->item;
		if ((victim->prev) && (victim->next)) {
			victim->prev->next = victim->next;
			victim->next->prev = victim->prev;
		}
		else 
			if (victim->next) {
				victim->next->prev = 0;
				head = victim->next;
			}
			else 
				if (victim->prev) {
					victim->prev->next = 0;
					tail = victim->prev;
				}
				else {
					head = 0;
					tail = 0;
				}
		delete victim;
		return i;
	}

	/**
	 * Return the nth element of the list.
	 *
	 * @param n
	 *    points to the elements (starting from 0) to return.
	 * @return 
	 *    a pointer to a list item, or 0 if the list does contain 
	 *   less than n elements.
	 */
	T* getNth(int n) const {
		if (!head)
			return NULL; 
		else {
			ListItem<T> *p = head; 

			while ((n>0) && (p->next)) {
				n--;
				p = p->next;
			}
			if (n != 0)
				return NULL;
			return p->item;
		}
	}

	/**
	 * Return the position of a given element.
	 *
	 * @param element
	 *    a pointer to a list element.
	 * @return 
	 *    the list item holding the given element, or 0 if element
	 *    is not part of the list.
	 */
	ListItem<T>* position(T* t) const {

		ListItem<T> *p = head; 
		
		while ((p) && (p->item != t))
			p = p->next;
		return p;
	}
		
	/**
	 * Return the index of a given element.
	 *
	 * @param element
	 *    a pointer to a list element.
	 * @return
	 *    the index (counted from 0) of element within the receiver.
	 *    If the receiver does not contain element, then -1 is returned.
	 */
	int index(T* t) const {

		ListItem<T> *p = head; 
		int i=0;
		for (; (p) && (p->item != t); i++)
			p = p->next;
		return (p) ? i : -1;
	}
		
	/**
	 * Return the first element.
	 *
	 * @return
	 *    the pointer to the first element, or 0 if the receiver is empty.
	 */
	T* first() const        { return (head) ? head->item : 0; }
	/**
	 * Return the last element.
	 *
	 * @return
	 *    the pointer to the last element, or 0 if the receiver is empty.
	 */
	T* last() const		{ return (tail) ? tail->item : 0; }

	/**
	 * Replace the nth element with a given one.
	 *
	 * @param n
	 *    the index of the element to replace (counted from 0).
	 * @param element
	 *    a pointer to an element.
	 * @return 
	 *    the pointer to the new element at index n, or 0 if the 
	 *    receiver contains less than n elements.
	 */
	T* overwriteNth(int n, T* t) {
		if (!head)
			return NULL; 
		else {
			ListItem<T> *p = head; 

			while ((n>0) && (p->next)) {
				n--;
				p = p->next;
			}
			if (n == 0) {
				if (p->item)
					delete p->item;
				p->item = t;
			}
			return p->item;
		}
	}

	/**
	 * Remove the n elements from the end of the receiver.
	 *
	 * @param n
	 *    the number of elements to be removed.
	 * @return 
	 *    the count of successfully removed elements.
	 */
	int trim(int n) {
		T* t = 0;
		int i=0;
		for (; (i<n) && ((t = removeLast()) != 0); i++) {
			delete t;
		}
		return i;
	}

	/**
	 * Get the element count of the list.
	 *
	 * @return
	 *    the receiver's size.
	 */
	int size() const {
		int r = 0;
		ListItem<T> *p = head;
		while (p) {
			r++;
			p = p->next;
		}
		return r;
	}

	/**
	 * Check whether the list is empty.
	 *
	 * @return
	 *    a value != 0, if the list is empty. A value of 0, if the
	 *    list is not empty.
	 */
	bool empty() const {
		return (head == 0);
	}

	/**
	 * Default constructor.
	 */
	List(): head(0), tail(0) { }
	
	/**
	 * Destructor (deletes all elements of the list)
	 */
	~List()						{ clearAll(); }

protected:

	ListItem<T> *head;
	ListItem<T> *tail;
};

template <class T> class ListCursor {
public:
	void init(const List<T>* l)	       	{ cursor = l->head; }
	void initLast(const List<T>* l)	       	{ cursor = l->tail; }
	void init(ListItem<T>* t)		{ cursor = t; }

	void init(const OrderedList<T>* l) { cursor = l->content.head; }
	void initLast(const OrderedList<T>* l) { cursor = l->content.tail; }
	
	T* get() {
		if (cursor)	
			return cursor->item;
		else
			return 0;
	}

	int next() {
		if ((cursor) && (cursor->next)) {
			cursor = cursor->next;
			return 1;
		} else 
		        if (cursor)
				cursor = 0;
		return 0;	
	}

	int prev() {
		if ((cursor) && (cursor->prev)) {
			cursor = cursor->prev;
			return 1;
		} else 
		        if (cursor)
				cursor = 0;
		return 0;	
	}

	int isNext() {
		if ((cursor) && (cursor->next)) 
			return 1;
		return 0;	
	}

	int isPrev() {
		if ((cursor) && (cursor->prev)) 
			return 1;
		return 0;	
	}

	ListItem<T>* get_cursor() {
		return cursor;
	}

	ListCursor(): cursor(0) { };
	ListCursor(const List<T>* l): cursor(l->head) { };
	ListCursor(const ListCursor<T>& c): cursor(c.cursor) { };
	
protected:
	ListItem<T>	*cursor;
};


//
// OrderedList is implemented as a wrapper of List to avoid corrupting
// the order by using directly methods of List
//

template <class T> class OrderedList;

template <class T> class OrderedList {
friend class ListCursor<T>;
public:
	T* add(T* item) {
		if (!content.empty()) {
			ListCursor<T> cur;
			for (cur.init(&content); cur.get(); cur.next()) {
				if (*item < *cur.get())
					return content.insertBefore(item, cur.get());
			}
			return content.add(item);
		}
		else return content.add(item);
	}

	T* addLast(T* item) {
		if (!content.empty()) {
			ListCursor<T> cur;
			for (cur.initLast(&content); cur.get(); cur.prev()) {
				if (*item > *cur.get())
					return content.insertAfter(item, 
								   cur.get());
			}
			return content.addFirst(item);
		}
		else return content.addFirst(item);
	}

	// add a new item, but if an equal item already exists:
	// do not change the list, delete the new item and return 0 
	T* addUnique(T* item) {
		if (!content.empty()) {
			ListCursor<T> cur;
			for (cur.init(&content); cur.get(); cur.next()) {
				if (*item == *cur.get()) {
					delete item;
					return 0;
				}
				if (*item < *cur.get()) 
					return content.insertBefore(item, cur.get());
			}
			return content.add(item);
		}
		else return content.add(item);
	}

	T* remove(T* item) { return content.remove(item); }
	T* remove(ListItem<T>* item) { return content.remove(item); }

	T* overwriteNth(int n, T* t) {
		return content.overwriteNth(n, t);
	}

	int	size() const  { return content.size(); }
	T*	first() const { return content.first(); }
	T*	last() const  { return content.last(); }
	T*	getNth(int i) const  { return content.getNth(i); }

	ListItem<T>* position(T* t) const { return content.position(t); }
	int		index(T* t) const { return content.index(t); }

	int	empty() const { return content.empty(); }

	void	clearAll()	{ content.clearAll(); }
	void	clear()		{ content.clear(); }

protected:

	List<T>	content;	
};


template <class T> class OrderedListCursor {
public:
	void init(const OrderedList<T>* l)     { cursor.init(l); }
	void initLast(const OrderedList<T>* l) { cursor.initLast(l); }
	void init(ListItem<T>* t)	       { cursor.init(t); }
	
	T*	get()		{ return cursor.get(); }
	int	next()		{ return cursor.next(); }
	int	isNext()	{ return cursor.isNext(); }
	int	prev()		{ return cursor.prev(); }
	int	isPrev()	{ return cursor.isPrev(); }
	ListItem<T>* get_cursor() { return cursor.get_cursor(); }
	
	OrderedListCursor() { }
	OrderedListCursor(const OrderedList<T>* l): 
	  cursor((const List<T>*)l) { }
	OrderedListCursor(const OrderedListCursor<T>& c): cursor(c.cursor) { }
protected:
	ListCursor<T> cursor;
};




template <class T> class OidList {
friend class OidListCursor<T>;
public:
	T* add(T* item) {
		(*content)[item->key()] = (void*)item;
		return item;
	}

	T* remove(T* item) {
		content->del(item->key());
		return item;
	}

	void remove(Oidx* oidptr) {
		T* t = find(oidptr);
		content->del(oidptr);
		if (t) delete t;
	}	  

	T* find(Oidx* oidptr) const {
		Pix i = content->seek(oidptr);
		if (i) return (T*)content->contents(i);
		return 0;
	}

	T* find_lower(Oidx* oidptr) const {
		Pix i = content->seek_inexact(oidptr);
		if (!i) return 0;
		while ((i) && (i != content->last()) &&
		       (*content->key(i) < *oidptr))
			content->next(i);
		while ((i) && (*content->key(i) > *oidptr))
			content->prev(i);
		
		if (i) return (T*)content->contents(i);
		return 0;
	}

	T* find_upper(Oidx* oidptr) const {
		Pix i = content->seek_inexact(oidptr);
		if (!i) return 0;
		while ((i) && (i != content->first()) &&
		       (*content->key(i) > *oidptr))
			content->prev(i);
		while ((i) && (*content->key(i) < *oidptr))
			content->next(i);
		if (i) return (T*)content->contents(i);
		return 0;
	}

	T* find_next(Oidx* oidptr) const {
		Pix i = content->seek(oidptr);
		if (!i) return 0;
		content->next(i);
		if (i) return (T*)content->contents(i);
		return 0;
	}

	T* find_prev(Oidx* oidptr) const {
		Pix i = content->seek(oidptr);
		if (!i) return 0;
		content->prev(i);
		if (i) return (T*)content->contents(i);
		return 0;
	}

	T* seek(Oidx* oidptr) const {
		Pix i = content->seek_inexact(oidptr);
		if (i) return (T*)content->contents(i);
		return 0;
	}

	int	size() const  { return content->length(); }

	T*	first() const {
		Pix i = content->first();
		if (i) return (T*)content->contents(i);
		return 0;
	}

	T*	last() const {
		Pix i = content->last();
		if (i) return (T*)content->contents(i);
		return 0;
	}

	T*	getNth(int i) const {
		Pix x = content->first();
		for (int n = 0; ((n<i) && (x)); n++) {
			content->next(x);
		}
		if (x) return (T*)content->contents(x);
		return 0;
	}

	int     index(T* t) const { 
		Pix i = content->seek(t->key());
		int n = -1;
		while (i) {
			content->prev(i);
			n++;
		}
		return n;
	}

	bool	empty() const { return content->empty(); }

	void	clear() { 
		content->clear();
	}
	void	clearAll()	{
		Pix i = content->first();
		while (i) {
			T* t = (T*)content->contents(i);
			content->next(i);
			content->del(t->key()); 
			delete t;
		}
		content->clear();
	}

	OidList() { 
		content = new OidxPtrEntryPtrAVLMap(0); 
	}
	~OidList() { clearAll(); delete content; }

protected:

	OidxPtrEntryPtrAVLMap*	content;	
};


template <class T> class OidListCursor {
public:
	void init(OidList<T>* l) { 
		list = l;
		cursor = l->content->first(); 
	}
	
	void initLast(OidList<T>* l) { 
		list = l;
		cursor = l->content->last(); 
	}

	T* get() {
		if ((list) && (cursor))	
			return (T*)list->content->contents(cursor);
		else
			return 0;
	}

	T* get_next() {
		if ((list) && (cursor)) {
			Pix x = cursor;
			list->content->next(x);
			if (x)
				return (T*)list->content->contents(x);
		}
		return 0;
	}

	T* get_prev() {
		if ((list) && (cursor)) {
			Pix x = cursor;
			list->content->prev(x);
			if (x)
				return (T*)list->content->contents(x);
		}
		return 0;
	}

	int next() {
		if ((list) && (cursor)) {
			list->content->next(cursor);
			return (cursor) ? TRUE : FALSE;
		} else 
		        if (cursor)
				cursor = 0;
		return 0;	
	}

	int prev() {
		if ((list) && (cursor)) {
			list->content->prev(cursor);
			return (cursor) ? TRUE : FALSE;
		} else 
		        if (cursor)
				cursor = 0;
		return 0;	
	}

	int lookup(Oidx* oidptr) {
		if (list) {
			Pix i = list->content->seek_inexact(oidptr);
			if (!i) return FALSE;
			T* t = 0;
			while ((i) && (t = (T*)list->content->contents(i)) &&
			       (*t->key() > *oidptr)) {
				list->content->prev(i);
			}
			if ((i) && (t)) { 
				cursor = i;
				return TRUE;
			}
		}
		return FALSE;
	}

	OidListCursor(): cursor(0) { list = 0; }
	OidListCursor(OidList<T>* l): 
	  cursor(l->content->first()) { list = l; }
	OidListCursor(const OidListCursor<T>& c): cursor(c.cursor) 
	  { list = c.list; }
protected:
	Pix	 cursor;
	OidList<T>* list;
};


/**
 * This Array template implements a vector collection class. 
 * 
 * @version 4.0.0
 * @author Frank Fock
 */
template <class T> class Array {
public:
	/**
	 * Add an element at the array's beginning.
	 *
	 * @param t
	 *    a pointer to an instance to be added.
	 * @return 
	 *    the same pointer t
	 */ 
	T* addFirst(T* t) { 
		T** h = content;
		content = new T*[sz+1];
		memcpy(content+1, h, sz*sizeof(T*));
		content[0] = t;
                if (h) {
                        delete[] h;
                }
		sz++;
		return t;
	}

	/**
	 * Add an element at the array's end.
	 *
	 * @param t
	 *    a pointer to an instance to be added.
	 * @return 
	 *    the same pointer t
	 */ 
        T* addLast(T* t) {
		T** h = content;
		content = new T*[sz+1];
		memcpy(content, h, sz*sizeof(T*));
		content[sz++] = t;
                if (h) {
                        delete[] h;
                }
		return t;
	}

	/**
	 * Add an element at the list's end.
	 *
	 * @param t
	 *    a pointer to an instance to be added.
	 * @return 
	 *    the same pointer t
	 */ 
	T* add(T* t)	{ return addLast(t); }

	/**
	 * Insert an element before a given element.
	 *
	 * @param newElement
	 *    the element to add.
	 * @param element
	 *    a pointer to an element of the list. If the receiver does
	 *    not contain such an element, then item is appended to the 
	 *    list.
	 */
        T* insertBefore(T* item, T* elem) {
		for (unsigned int i=0; i<sz; i++) {
			if (content[i] == elem) {
				if (i == 0) return addFirst(item);
				T** h = content;
				content = new T*[sz+1];
				memcpy(content, h, i*sizeof(T*));
				memcpy(content+i+1, h+i, 
				       (sz-i)*sizeof(T*)); 
				content[i] = item;
                                if (h) {
                                        delete[] h;
                                }
				sz++;
				return item;			
			}
		} 
		return addLast(item);
	}

	/**
	 * Insert an element after a given element.
	 *
	 * @param newElement
	 *    the element to add.
	 * @param element
	 *    a pointer to an element of the list. If the receiver does
	 *    not contain such an element, then item is appended to the 
	 *    list.
	 */
        T* insertAfter(T* item, T* elem) {
		for (unsigned int i=0; i<sz; i++) {
			if (content[i] == elem) {
				if (i == sz-1) return addLast(item);
				T** h = content;
				content = new T*[sz+1];
				memcpy(content, h, (i+1)*sizeof(T*));
				if (i+1<sz) 
					memcpy(content+i+2, h+i+1, 
					       (sz-i-1)*sizeof(T*)); 
				content[i+1] = item;
                                if (h) {
                                        delete[] h;
                                }
				sz++;
				return item;			
			}
		} 
		return addLast(item);
	}

	/**
	 * Remove the first element.
	 *
	 * @return 
	 *    a pointer to the removed element or 0 if the list is empty.
	 */
	T* removeFirst() {
		if (sz == 0) return 0; 
		T* t = content[0];
		T** h = content;
		content = new T*[--sz];
		memcpy(content, h+1, sz*sizeof(T*));
                if (h) {
                        delete[] h;
                }
		return t;
	}

	/**
	 * Remove the last element.
	 *
	 * @return 
	 *    a pointer to the removed item or 0 if the list is empty.
	 */
	T* removeLast() {
		if (sz > 0)
			return content[--sz];
		return 0;
	}

	/**
	 * Remove and delete all elements from the receiver.
	 */
	void clearAll() {
		for (unsigned int i=0; i<sz; i++) {
			delete content[i];
		}
		delete[] content;
		sz = 0;
		content = 0;
	}

	/**
	 * Empty the receiver list without freeing the memory of the
	 * list elements.
	 */
	void clear() {
		delete[] content;
		sz = 0;
		content = 0;
	}

	/**
	 * Clear a specified element, thus the element pointer is set
	 * to 0.
	 *
	 * @param i
	 *    the index of the element to clear.
	 */
	void clear(int i) {
		content[i] = 0;
	}

	/**
	 * Remove an element.
	 * 
	 * @param item
	 *    a pointer to a list item.
	 * @return
	 *    the pointer to the given item, or 0 if the receiver does not
	 *    contain item.
	 */
	T* remove(T* item) {
		for (unsigned int i=0; i<sz; i++) {
			if (item == content[i]) {
				return remove(i);
			}
		}
		return 0;
	}

	/**
	 * Remove an element using a cursor.
	 * Note: The cursor is invalid after this operation!
	 * 
	 * @param item
	 *    a pointer to a list cursor.
	 * @return
	 *    the pointer to the removed item.
	 */
	T* remove(unsigned int i) {
		if (i >= sz) return 0;
		T* t = content[i];
		T** h = content;
		content = new T*[sz-1];
		if (i > 0)
			memcpy(content, h, i*sizeof(T*));
		if (i+1 < sz)
			memcpy(content+i, h+i+1,(sz-i-1)*sizeof(T*));
                if (h) {
                        delete[] h;
                }
		sz--;
		return t;
	}

	/**
	 * Return the nth element of the list.
	 *
	 * @param n
	 *    points to the elements (starting from 0) to return.
	 * @return 
	 *    a pointer to a list item, or 0 if the list does contain 
	 *   less than n elements.
	 */
	T* getNth(int n) const {
		if ((n < 0) || (((unsigned int)n) >= sz))  return 0; 
		return content[n];
	}

	/**
	 * Return the index of a given element.
	 *
	 * @param element
	 *    a pointer to a list element.
	 * @return
	 *    the index (counted from 0) of element within the receiver.
	 *    If the receiver does not contain element, then -1 is returned.
	 */
	int index(T* t) const {
		for (unsigned int i=0; i<sz; i++) {
			if (t == content[i]) 
				return i;
		}
		return -1;
	}
		
	/**
	 * Return the first element.
	 *
	 * @return
	 *    the pointer to the first element, or 0 if the receiver is empty.
	 */
	T* first() const        { return (sz) ? content[0] : 0; }
	/**
	 * Return the last element.
	 *
	 * @return
	 *    the pointer to the last element, or 0 if the receiver is empty.
	 */
	T* last() const		{ return (sz) ? content[sz-1] : 0; }

	/**
	 * Replace the nth element with a given one.
	 *
	 * @param n
	 *    the index of the element to replace (counted from 0).
	 * @param element
	 *    a pointer to an element.
	 * @return 
	 *    the pointer to the new element at index n, or 0 if the 
	 *    receiver contains less than n elements.
	 */
	T* overwriteNth(int n, T* t) {
		if ((n < 0) || ((unsigned int)n >= sz)) return 0;
		if (content[n]) delete content[n];
		content[n] = t;
		return content[n];
	}

	T& operator[](int n) const {
		return *(content[n]);
	}

	/**
	 * Remove the n elements from the end of the receiver.
	 *
	 * @param n
	 *    the number of elements to be removed.
	 * @return 
	 *    the count of successfully removed elements.
	 */
	int trim(int n) {
		int i=0;
		for (i=0; (i<n) && (sz > 0); i++) {
			T* t = removeLast();
			if (t) delete t;
			else break;
		}
		return i;
	}

	/**
	 * Get the element count of the list.
	 *
	 * @return
	 *    the receiver's size.
	 */
	int size() const {
		return sz;
	}

	/**
	 * Check whether the list is empty.
	 *
	 * @return
	 *    a value != 0, if the list is empty. A value of 0, if the
	 *    list is not empty.
	 */
	int empty() const {
		return (sz == 0);
	}

	/**
	 * Clone the array.
	 */
	Array<T>* clone() {
		Array<T>* r = new Array<T>();
		if (sz == 0) return r;
		r->sz = sz;
		delete[] r->content;
		r->content = new T*[sz];
		for (unsigned int i=0; i<sz; i++) {
			r->content[i] = (T*)content[i]->clone();
		}
		return r;
	}

	Array<T>& operator=(const Array<T>& o) {
		if (this == &o) return *this;
		clearAll();
		sz = o.sz;
		content =  new T*[sz];
		for (unsigned int i=0; i<sz; i++) {
			content[i] = (T*)o.content[i]->clone();
		}
		return (*this);		
	}

	/**
	 * Default constructor.
	 */
	Array(): sz(0) { content = 0; }
	
	/**
	 * Destructor (deletes all elements of the list)
	 */
	~Array()	{ clearAll(); }

protected:

	T**		content;
	unsigned int	sz;
};

template <class T> class OrderedArray;

template <class T> class ArrayCursor {
public:
	void init(const Array<T>* l) { 
		cursor = 0; 
		list = l;
	}

	void initLast(const Array<T>* l) { 
		list = l;
		cursor = l->size()-1; 
	}

	T* get() {
		if ((cursor < 0) || !list || (cursor >= list->size())) return 0;
		return list->getNth(cursor);
	}

	int next() {
		if ((++cursor >= list->size())) return 0;
		return 1;	
	}

	int prev() {
		if (--cursor < 0) return 0;
		return 1;
	}

	int isNext() {
		if ((cursor+1 >= list->size())) return 0;
		return 1;	
	}

	int isPrev() {
		if (cursor-1 < 0) return 0;
		return 1;
	}

	int get_cursor() {
		return cursor;
	}

	ArrayCursor(): cursor(0) { list = 0; };
	ArrayCursor(const List<T>* l): cursor(0) { list = l; };
	ArrayCursor(const ArrayCursor<T>& c): cursor(c.cursor) 
	  { list = c.list; };
	
protected:
	const Array<T>*	list;
        int		cursor;
};

//
// OrderedList is implemented as a wrapper of List to avoid corrupting
// the order by using directly methods of List
//

template <class T> class OrderedArray;

template <class T> class OrderedArray: public Array<T> {
friend class ArrayCursor<T>;
public:
	T* addBegin(T* item) {
		if (!this->empty()) {
			for (unsigned int i=0; i<this->sz; i++) {
				if (*item < *(this->content[i]))
					return this->insertBefore(item, 
							    this->content[i]);
			}
			return Array<T>::add(item);
		}
		else return Array<T>::add(item);
	}

	T* add(T* item) {
		return addEnd(item);
	}

	T* addEnd(T* item) {
		if (!this->empty()) {
			for (int i=this->sz-1; i>=0; i--) {
				if (*item > *(this->content[i]))
					return this->insertAfter(item, 
							   this->content[i]);
			}
			return Array<T>::addFirst(item);
		}
		else return Array<T>::addFirst(item);
	}

	// add a new item, but if an equal item already exists:
	// do not change the list, delete the new item and return 0 
	T* addUnique(T* item) {
		if (!this->empty()) {
			for (unsigned int i=0; i<this->sz; i++) {
				if (*item == *(this->content[i])) {
					delete item;
					return 0;
				}
				if (*item < *(this->content[i]))
					return this->insertBefore(item, 
							    this->content[i]);
			}
			return Array<T>::add(item);
		}
		else return Array<T>::add(item);
	}

};

#ifdef AGENTPP_NAMESPACE
}
#endif

#endif
