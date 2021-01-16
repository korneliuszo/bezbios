/*
 * tlay2.hpp
 *
 *  Created on: Jan 5, 2021
 *      Author: Korneliusz Osmenda
 */

#ifndef UART_TLAY2_HPP_
#define UART_TLAY2_HPP_

#include <sched/bezbios_sched_api.h>

class Tlay2
{
private:
	void (*const putch)(unsigned char);
	unsigned char (* const getch)();
	unsigned char buff[1024];
	unsigned char send_crc;
	void put_escaped(unsigned char c);
public:
	unsigned char ctr;
	unsigned char pid;
	unsigned char * payload;
	long len;
	BezBios::Sched::Mutex send_mutex;
	void reply(const unsigned char * payload, long len);
	void msgout(unsigned char ctr,unsigned char pid, const unsigned char * payload, long len);
	void dbgout(const char * msg);
	Tlay2(void (*_putch)(unsigned char), unsigned char (*_getch)() );
	void process();
};

struct Tlay2Payloads
{
	const unsigned char pid;
	void (*parse)(Tlay2 &);
};

extern struct Tlay2Payloads tlay2_payloads[];

extern Tlay2 tlay2_uart;

void tlay2_monitor(Tlay2 &);

#endif /* UART_TLAY2_HPP_ */
