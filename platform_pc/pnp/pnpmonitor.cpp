/*
 * pnpmonitor.cpp
 *
 *  Created on: Sep 19, 2021
 *      Author: Korneliusz Osmenda
 */
#include "pnpmonitor.hpp"
#include "pnp.hpp"
#include "uart/endianbuff.h"

void tlay2_pnp_monitor(Tlay2 & tlay2)
{
	switch(tlay2.payload[0])
	{
	case 0x00: // get pnpinfo
		if (tlay2.len == 1)
		{
			unsigned short no_nodes;
			unsigned short max_size;
			unsigned short result = pnp.get_node_info(&no_nodes,&max_size);
			unsigned char buff[5];
			put_short_le(&buff[0],result);
			buff[2] = no_nodes;
			put_short_le(&buff[3],max_size);
			tlay2.reply(buff,5);
		}
		break;
	}
}
