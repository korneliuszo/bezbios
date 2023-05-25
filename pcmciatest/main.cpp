/*
 * main.cpp
 *
 *  Created on: Nov 16, 2020
 *      Author: Korneliusz Osmenda
 */

#include <sched/bezbios_sched_api.h>
#include <uart/bezbios_uart_api.h>
#include <interrupts.h>
#include <uart/tlay2.hpp>
#include <display/bezbios_display_api.h>
#include <uart/monitor.hpp>
#include <uart/vm86_monitor.hpp>
#include <uart/endianbuff.h>
#include <vm86.hpp>
#include <apm.hpp>
#include <yenta_socket.hpp>

YentaSocket sock1;
YentaSocket sock2;

int second_stack[4096];

static BEZBIOS_INIT_PIT delay_implementation;

struct Tlay2Payloads tlay2_payloads[] = {
		{1,tlay2_monitor},
		{2,tlay2_v86_monitor},
		{0,nullptr} // terminator
};

void third()
 {
	//char leters[]="Bios\n";
	while(1)
	{
		bezbios_delay_ms(1000);
		//tlay2_uart.dbgout(leters);
	}

 }
static BEZBIOS_CREATE_PROCESS<third,4096> third_thread;


static long monitor_shutdown(unsigned char [],long)
{
	apm.shutdown();
	tlay2_uart.dbgout("wolololo"); //trytytka na used tlay2_uart
	return 0;
}

const MonitorFunctions monitor_functions[] = {

		{3,MonitorFunctions::Type::ARRAY_ARGUMENTS, monitor_shutdown},
		{0,MonitorFunctions::Type::TERMINATOR, nullptr}
};
