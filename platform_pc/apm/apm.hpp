/*
 * apm.hpp
 *
 *  Created on: Sep 7, 2021
 *      Author: Korneliusz Osmenda
 */

#ifndef PLATFORM_PC_APM_APM_HPP_
#define PLATFORM_PC_APM_APM_HPP_

class APM {
	bool call(unsigned short a, unsigned short b, unsigned short c);
	unsigned long offset;
	public:
	APM();
	void shutdown(void);
};

extern APM apm;

#endif /* PLATFORM_PC_APM_APM_HPP_ */
