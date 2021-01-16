/*
 * tlay2.cpp
 *
 *  Created on: Dec 30, 2020
 *      Author: Korneliusz Osmenda
 */

/*
 * protocol
 *  *-----*---------*--------*------------*
 *  | ctr | fn addr | n*data | CRC8 CCITT |
 *  *-----*---------*--------*------------*
 */

#include "tlay2.hpp"
#include "crc8.h"


Tlay2::Tlay2(void (*_putch)(unsigned char), unsigned char (*_getch)() )
	: putch(_putch)
	, getch(_getch)
	, send_crc(0)
	,ctr(0)
	,pid(0)
	,payload(nullptr)
	,len(0)
	{}

void Tlay2::process()
{
	while(1)
	{
reset:
		unsigned int l = 0;
		crc_t crc = crc_init();
		for (unsigned char c = getch(); c != '\n'; c=getch())
		{
			if(c == 0xdc)
			{
				c = getch();
				c^=0x80;
			}
			crc=crc_update(crc,&c,1);
			buff[l++] = c;
			if (l == sizeof(buff))
			{
				while(getch() != '\n');
				goto reset;
			}
		}

		if(crc_finalize(crc) != 0)
		{
			goto reset;
		}
		if(l < 3)
		{
			goto reset;
		}
		ctr = buff[0];
		pid = buff[1];
		payload = &buff[2];
		len = l - 2 - 1;
		for(const struct Tlay2Payloads* tp = tlay2_payloads;tp->pid;tp++)
		{
			if(tp->pid == pid)
			{
				tp->parse(*this);
				break;
			}
		}
	}
}

void Tlay2::reply(const unsigned char * payload, long len)
{
	msgout(ctr,pid,payload,len);
}

void Tlay2::put_escaped(unsigned char c)
{
	send_crc = crc_update(send_crc, &c, 1);
	if (c == '\n' || c == 0xdc)
	{
		putch(0xdc);
		c ^=0x80;
	}
	putch(c);
}

void Tlay2::msgout(unsigned char ctr,unsigned char pid, const unsigned char * payload, long len)
{
	send_mutex.aquire();
	send_crc = crc_init();
	put_escaped(ctr);
	put_escaped(pid);
	for(long i=0;i<len;i++)
		put_escaped(payload[i]);
	put_escaped(send_crc);
	putch('\n');
	send_mutex.release();
}

void Tlay2::dbgout(const char * msg)
{
	send_mutex.aquire();
	send_crc = crc_init();
	put_escaped(0);
	put_escaped(0);
	while(*msg)
		put_escaped(*(msg++));
	put_escaped(send_crc);
	putch('\n');
	send_mutex.release();
}
