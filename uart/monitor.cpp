/*
 * monitor.cpp
 *
 *  Created on: Jan 6, 2021
 *      Author: Korneliusz Osmenda
 */

#include "tlay2.hpp"
#include "io.h"
#include "endianbuff.h"

static unsigned char getmem_buff[512];

void tlay2_monitor(Tlay2 & tlay2)
{
	switch(tlay2.payload[0])
	{
	case 0x00: //PING
		tlay2.reply(tlay2.payload,tlay2.len);
		break;
	case 0x01: // outb
		if (tlay2.len == 4)
		{
			outb(get_short_le(&tlay2.payload[1]),tlay2.payload[3]);
			io_wait(0x01);
			tlay2.reply(nullptr,0);
		}
		break;
	case 0x02: //inb
		if (tlay2.len == 3)
		{
			unsigned char ret=inb(get_short_le(&tlay2.payload[1]));
			tlay2.reply(&ret,1);
		}
		break;
	case 0x03: // putmem
		if(tlay2.len >= 5)
		{
			volatile unsigned char * addr =
					(volatile unsigned char*) get_long_le(&tlay2.payload[1]);
			for(long i=5;i<tlay2.len;i++)
				*(addr++) = tlay2.payload[i];
			tlay2.reply(nullptr,0);
		}
		break;
	case 0x04: //getmem
		if (tlay2.len == 7)
		{
			volatile unsigned char * addr =
					(volatile unsigned char*) get_long_le(&tlay2.payload[1]);
			unsigned short len = get_short_le(&tlay2.payload[5]);
			if (len > 512) break;
			for(long i=0;i<len;i++)
				getmem_buff[i] = *(addr++);
			tlay2.reply(getmem_buff,len);
		}
		break;
	}
}
