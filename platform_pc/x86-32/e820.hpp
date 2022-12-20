/*
 * e820.hpp
 *
 *  Created on: 20 gru 2022
 *      Author: nyuu
 */

#ifndef PLATFORM_PC_X86_32_E820_HPP_
#define PLATFORM_PC_X86_32_E820_HPP_

#include <stdint.h>

struct __attribute__ ((__packed__)) e820_data
{
	uint64_t base;
	uint64_t len;
	uint32_t flags;
};

struct e820_ret
{
	e820_data data;
	uint32_t cont;
	bool success;
};


e820_ret e820_call(uint32_t cont);


#endif /* PLATFORM_PC_X86_32_E820_HPP_ */
