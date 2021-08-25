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

static
void first()
{
	tlay2_uart.dbgout("LOADER_WORKING!");
}


static BEZBIOS_CREATE_PROCESS<first,4096> first_thread;
static BEZBIOS_INIT_PIT delay_implementation;

struct Tlay2Payloads tlay2_payloads[] = {
		{1,tlay2_monitor},
		{2,tlay2_v86_monitor},
		{0,nullptr} // terminator
};

static long monitor_init_display(unsigned char [],long len)
{
	if (len !=0)
		return -1;
	init_display();
	return 0;
}

static long monitor_put_next_pixels(unsigned char data[],long len)
{
	if (len < 4)
		return -1;
	int x= get_short_le(&data[0]);
	int y= get_short_le(&data[2]);
	put_next_pixels(x,y,len-4,&data[4]);
	return 0;
}

static long monitor_put_palette(unsigned char data[],long len)
{
	if (len < 1)
		return -1;
	int start = data[0];
	put_palette(start,(len-1)/3,&data[1]);
	return 0;
}

const MonitorFunctions monitor_functions[] = {
		{0,MonitorFunctions::Type::ARRAY_ARGUMENTS, monitor_init_display},
		{1,MonitorFunctions::Type::ARRAY_ARGUMENTS, monitor_put_next_pixels},
		{2,MonitorFunctions::Type::ARRAY_ARGUMENTS, monitor_put_palette},
		{0,MonitorFunctions::Type::TERMINATOR, nullptr}
};
