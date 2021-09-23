/*
 * pnp.hpp
 *
 *  Created on: Sep 8, 2021
 *      Author: Korneliusz Osmenda
 */

#ifndef PLATFORM_PC_PNP_PNP_HPP_
#define PLATFORM_PC_PNP_PNP_HPP_

#include "vm86.hpp"

class PnP {
	unsigned short call(
		unsigned short func,
		unsigned short arg1,
		unsigned short arg2,
		unsigned short arg3,
		unsigned short arg4,
		unsigned short arg5,
		unsigned short arg6,
		unsigned short arg7);
	LONGADDR rm;
	unsigned short ds;
	bool found;
	public:
	PnP();
	unsigned short get_node_info(unsigned short *no_nodes, unsigned short *max_size);
	unsigned short get_system_node(unsigned char *no_node, unsigned char **buff);
};

extern PnP pnp;







#endif /* PLATFORM_PC_PNP_PNP_HPP_ */
