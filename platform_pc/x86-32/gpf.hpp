/*
 * gpf.hpp
 *
 *  Created on: Aug 19, 2021
 *      Author: Korneliusz Osmenda
 */

#ifndef PLATFORM_PC_X86_32_GPF_HPP_
#define PLATFORM_PC_X86_32_GPF_HPP_

#include "interrupts.h"

template<typename LOSTACK>
struct __attribute__((packed)) User_stack : LOSTACK {
	unsigned long esp;
	unsigned long ss;
};

static_assert(sizeof(User_stack<Error_stack>) == 68, "Verifying size failed!");


#endif /* PLATFORM_PC_X86_32_GPF_HPP_ */
