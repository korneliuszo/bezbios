/*
 * cwrap.hpp
 *
 *  Created on: 4 sty 2023
 *      Author: nyuu
 */

#ifndef SCHED_CWRAP_HPP_
#define SCHED_CWRAP_HPP_


template<class C,  void(C::*F)()>
void Cwrap(void* cobj)
{
    C* obj = (C*)cobj;
    (obj->*F)();
    return;
}


#endif /* SCHED_CWRAP_HPP_ */
