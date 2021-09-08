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

/*class Test
{
	int test;
	Test() { test=4;};
};*/

int second_stack[4096];

char leters[]= "Bez\n";

typedef BezBios::Sched::ForYield<char *> Yieldcptr;

void second(Yieldcptr *yield)
{
	int i = 0;
	while(1)
	{
		yield->yield_data(&leters[i]);
		if (leters[i] == 0)
		{
			i=0;
			bezbios_delay_ms(1500);
		}
		else
		{
			i++;
		}
	}
}

static
void first()
{
	Yieldcptr yield;
	int stid=bezbios_sched_create_task((void(*)(void*))second,
			&second_stack[4095],
			&yield);
	yield.connect(stid);

	long i = 0;
	for (char * letter = yield.future_data();letter;letter = yield.future_data())
	{
		char buff[10];
		buff[i++] = *letter;
		if (*letter == 0)
		{
			tlay2_uart.dbgout(buff);
			i=0;
		}
	}
	bezbios_sched_destroy_task(stid);
}

void third()
 {
	char leters[]="Bios\n";
	while(1)
	{
		bezbios_delay_ms(1000);
		tlay2_uart.dbgout(leters);
	}

 }

static BEZBIOS_CREATE_PROCESS<first,4096> first_thread;
static BEZBIOS_CREATE_PROCESS<third,4096> third_thread;
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

static long monitor_shutdown(unsigned char [],long)
{
	apm.shutdown();
	return 0;
}

const MonitorFunctions monitor_functions[] = {
		{0,MonitorFunctions::Type::ARRAY_ARGUMENTS, monitor_init_display},
		{1,MonitorFunctions::Type::ARRAY_ARGUMENTS, monitor_put_next_pixels},
		{2,MonitorFunctions::Type::ARRAY_ARGUMENTS, monitor_put_palette},
		{3,MonitorFunctions::Type::ARRAY_ARGUMENTS, monitor_shutdown},
		{0,MonitorFunctions::Type::TERMINATOR, nullptr}
};
