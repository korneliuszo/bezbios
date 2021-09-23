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
	case 0x01: // get pnpnode
		if (tlay2.len == 2)
		{
			unsigned char * buff;
			unsigned char node = tlay2.payload[1];
			unsigned short result = pnp.get_system_node(&node,&buff);
			tlay2.replyinit();
			unsigned char retcode[3];
			put_short_le(&retcode[0],result);
			retcode[2] = node;
			tlay2.replyput(retcode,3);
			unsigned short len = *(unsigned short *)(&buff[0]);
			tlay2.replyput(buff,len);
			tlay2.replyend();
		}
		break;
	}
}
