/*
 * bios32.hpp
 *
 *  Created on: 29 gru 2022
 *      Author: nyuu
 */

#ifndef PLATFORM_PC_X86_32_BIOS32_HPP_
#define PLATFORM_PC_X86_32_BIOS32_HPP_

#include <stdint.h>

class BIOS32 {
private:
	BIOS32();
	unsigned long b32_entry;
public:
	static BIOS32 & get_handle();
	unsigned long get_service_entry(uint32_t service);
};







#endif /* PLATFORM_PC_X86_32_BIOS32_HPP_ */
