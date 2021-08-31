/*
 * tlay2_dbgout.hpp
 *
 *  Created on: Aug 31, 2021
 *      Author: Korneliusz Osmenda
 */

#ifndef UART_TLAY2_DBGOUT_HPP_
#define UART_TLAY2_DBGOUT_HPP_

#include <io.h>
#include "bezbios_uart_api_low.h"
#include "crc8.h"

class UartBlocking {
	unsigned long ss;
public:
	void start()
	{
		ss = cli();
	}
	void sendbyte(unsigned char byte)
	{
		bezbios_low_write_serial((char)byte);
	}
	void end()
	{
		sti(ss);
	}
	UartBlocking() : ss(0) {};
};

template <typename LOW>
class DbgOut
{
	LOW low;
	bool started;
	crc_t send_crc;
	void put_escaped(unsigned char c)
	{
		send_crc = crc_update(send_crc,&c, 1);
		if (c == '\n' || c == 0xdc)
		{
			low.sendbyte(0xdc);
			c ^=0x80;
		}
		low.sendbyte(c);
	}
	void putchar(unsigned char byte)
	{
		if(!started)
		{
			low.start();
			send_crc = crc_init();
			put_escaped(0);
			put_escaped(0);
			started=true;
		}
		put_escaped(byte);
	}
	public:
	DbgOut& end()
	{
		put_escaped(send_crc);
		low.sendbyte('\n');
		started = false;
		low.end();
		return *this;
	}
	template<typename T>
	DbgOut& hex(T val, long size=sizeof(T))
	{
		putchar('0');
		putchar('x');
		for(long i=size*2-1;i>=0;i--)
		{
			unsigned char nibble = (val>> (i*4))& 0xF;
			if (nibble < 10)
				putchar('0'+nibble);
			else
				putchar('A'+nibble-10);
		}
		return *this;
	}
	DbgOut& str(const char * msg)
	{
		while(*msg)
			putchar(*(msg++));
		return *this;
	}
	DbgOut() : low(),started(false),send_crc(0) {};
};





#endif /* UART_TLAY2_DBGOUT_HPP_ */
