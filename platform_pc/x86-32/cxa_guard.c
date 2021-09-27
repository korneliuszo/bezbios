/*
 * cxa_guard.c
 *
 *  Created on: 27 wrz 2021
 *      Author: nyuu
 */

#include "io.h"


int __cxa_guard_acquire(unsigned long long * guard_object)
{
	unsigned long flags = cli();
	if(*(unsigned char*)guard_object)
	{
		sti(flags);
		return 0;
	}
	*guard_object = ((unsigned long long)flags << 32) | 1;
	return 1;
}

void __cxa_guard_release(unsigned long long * guard_object) {
	sti(*guard_object>>32);
}
