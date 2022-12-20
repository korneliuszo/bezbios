/*
 * list.hpp
 *
 *  Created on: Dec 19, 2022
 *      Author: kosa
 */

#ifndef SCHED_LIST_HPP_
#define SCHED_LIST_HPP_

template <class T>
class List{
public:
	T *next;
	T *prev;
	List(bool looping) : next(looping ? static_cast<T*>(this) : nullptr), prev(looping ? static_cast<T*>(this) : nullptr){};
	List(T * obj, bool append) {
		plug(obj,append);
	}
	void unplug(){
		if(prev)
			prev->next = next;
		if(next)
			next->prev = prev;
		next = nullptr;
		prev = nullptr;
	}
	void plug(T * obj, bool append)
	{
		if(append)
		{
			prev = obj;
			if(obj->next)
				obj->next->prev = static_cast<T*>(this);
			next = obj->next;
			obj->next = static_cast<T*>(this);
		}
		else
		{
			next = obj;
			if(obj->prev)
				obj->prev->next = static_cast<T*>(this);
			prev = obj->prev;
			obj->prev = static_cast<T*>(this);
		}
	}
};







#endif /* SCHED_LIST_HPP_ */
