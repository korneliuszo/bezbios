/*
 * timerPIT.cpp
 *
 *  Created on: Dec 28, 2020
 *      Author: Korneliusz Osmenda
 */

#include "interrupts.h"
#include <sched/bezbios_sched_api.h>
#include "io.h"

static constexpr double PIT_FREQ = 18.2068;
static constexpr double PIT_INP_FREQ = 1193182.0;
static constexpr double PIT_RELOAD = PIT_INP_FREQ / PIT_FREQ;
static constexpr int PIT_RELOAD_INT = (PIT_RELOAD + 0.5);

static volatile unsigned long long systick_msf;
static constexpr unsigned long long systick_add =
		PIT_RELOAD_INT / PIT_INP_FREQ * 1000 *(1ULL<<32) + 0.5;

long bezbios_get_ms()
{
	asm volatile("cli");
	long ret = systick_msf >> 32;
	asm volatile("sti");
	return ret;
}

static void PIT_ISR(unsigned char irq)
{
	systick_msf +=systick_add;
	bezbios_int_ack(irq);
	int tid = bezbios_sched_get_tid();
	if(tid) // if in main/idle task we don't do preemption
	{
		bezbios_sched_task_ready(tid,1);
		bezbios_sched_free_cpu();
	}
}

void init_PIT() {
	outb(0x43,0x34);
	outb(0x40,PIT_RELOAD_INT &0xff);
	outb(0x40,PIT_RELOAD_INT >> 8);
	bezbios_hwirq_insert(0,PIT_ISR);
	bezbios_enable_irq(0);
}
