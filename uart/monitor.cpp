/*
 * monitor.cpp
 *
 *  Created on: Jan 6, 2021
 *      Author: Korneliusz Osmenda
 */

#include "tlay2.hpp"
#include "io.h"
#include "endianbuff.h"
#include "monitor.hpp"

static unsigned char getmem_buff[512];

static
long strlen(const unsigned char* start) {
   // NB: no nullptr checking!
   const unsigned char* end = start;
   for( ; *end != '\0'; ++end)
      ;
   return end - start;
}

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
	case 0x05: // outs
		if (tlay2.len == 5)
		{
			outs(get_short_le(&tlay2.payload[1]),get_short_le(&tlay2.payload[3]));
			io_wait(0x01);
			tlay2.reply(nullptr,0);
		}
		break;
	case 0x06: //ins
		if (tlay2.len == 3)
		{
			unsigned short ret=ins(get_short_le(&tlay2.payload[1]));
			unsigned char buff[2];
			put_short_le(buff,ret);
			tlay2.reply(buff,2);
		}
		break;
	case 0x07: //call //
		if (tlay2.len >= 2)
		{
			const MonitorFunctions *fun;
			for (fun=monitor_functions;
					fun->type != MonitorFunctions::Type::TERMINATOR;
					fun++)
				if(fun->funid == tlay2.payload[1])
					break;
			switch(fun->type)
			{
			case MonitorFunctions::Type::ARRAY_ARGUMENTS:
			{
				long len=fun->callback.array_arguments(&tlay2.payload[2],tlay2.len-2);
				if(len>=0)tlay2.reply(&tlay2.payload[2],len);
				break;
			}
			default:
			case MonitorFunctions::Type::TERMINATOR:
				break;
			}
		}
		break;
	case 0x08: //strget
		if(tlay2.len ==5)
		{
			unsigned char * addr =
					(unsigned char*) get_long_le(&tlay2.payload[1]);

			tlay2.reply(addr,strlen(addr));
		}
		break;
	case 0x09: // call raw
		if(tlay2.len ==5)
		{
			void (*addr)() =
					(void(*)()) get_long_le(&tlay2.payload[1]);
			addr();
		}
		break;
	}
}
