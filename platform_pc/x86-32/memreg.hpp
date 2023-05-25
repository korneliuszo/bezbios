/*
 * memreg.hpp
 *
 *  Created on: 19 gru 2022
 *      Author: nyuu
 */

#ifndef PLATFORM_PC_X86_32_MEMREG_HPP_
#define PLATFORM_PC_X86_32_MEMREG_HPP_

#include <sched/list.hpp>

class Memory_region : public List<Memory_region>{
protected:
	friend class Memory_pool;
	void *addr;
	unsigned long len;
public:
	Memory_region() : List(false){};
	void * get_addr() {return addr;}
	unsigned long get_len() {return len;}
	~Memory_region() { unplug();};
};

class Memory_pool {
public:
	void *addr;
	unsigned long len;
	Memory_region * first;
	bool allocate(Memory_region & region, unsigned long rlen, int align_shift)
	{
		unsigned long mask = (1<<align_shift)-1;
		unsigned long after_space = (unsigned long)addr;
		unsigned long after_aligned = (after_space + mask) & (~mask);
		Memory_region *newhead = first;
		if (!newhead) {
			if (after_aligned + rlen > (unsigned long) addr + len)
				return false;
			region.addr = (void*)after_aligned;
			region.len = rlen;
			first = &region;
			return true;
		} else {
			Memory_region *oldhead = nullptr;

			while (newhead) {
				after_space = (unsigned long) newhead->addr + newhead->len;
				after_aligned = (after_space + mask) & (~mask);
				if (!newhead->next
						&& after_aligned + rlen - len > (unsigned long) addr + len)
					return false;
				if (after_aligned + rlen < (unsigned long) newhead->next->addr)
					break;
				oldhead = newhead;
				newhead = newhead->next;
			}
			region.addr = (void*)after_aligned;
			region.len = rlen;
			if(first == newhead)
			{
				newhead->plug(&region, true);
				first = &region;
			}
			else
			{
				region.plug(oldhead, true);
			}
			return true;
		}
	}
};

#endif /* PLATFORM_PC_X86_32_MEMREG_HPP_ */
