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


struct DelayList {
	volatile long timeout;
	DelayList * volatile next;
};

static DelayList delay_tbl[CONFIG_MAX_THREADS];
static DelayList * volatile delay_head = nullptr;

long bezbios_get_ms()
{
	ENTER_ATOMIC();
	long ret = systick_msf >> 32;
	EXIT_ATOMIC();
	return ret;
}

template<>
__attribute__((interrupt))
void bezbios_imp_hw_req<0>::f(struct interrupt_frame *)
{
	systick_msf +=systick_add;

	long now = systick_msf >> 32;
	DelayList* newhead = delay_head;
	for (;
			newhead != nullptr;
			newhead = newhead->next)
	{
		if(newhead->timeout > now)
			break;
		bezbios_sched_task_ready(newhead - delay_tbl,1);
	}
	delay_head = newhead;

	bezbios_int_ack(0);
	int tid = bezbios_sched_get_tid();
	if(tid) // if in main/idle task we don't do preemption
	{
		bezbios_sched_free_cpu(1); //Round-Robin
	}
}

void init_PIT() {
	outb(0x43,0x34);
	outb(0x40,PIT_RELOAD_INT &0xff);
	outb(0x40,PIT_RELOAD_INT >> 8);
	bezbios_enable_irq(0);
}

void bezbios_delay_ms(int ms)
{
	int tid = bezbios_sched_get_tid();
	DelayList* ptr=&delay_tbl[tid];
	ENTER_ATOMIC();
	long now = bezbios_get_ms();
	long timeout = now + ms;
	ptr->timeout = timeout;

	DelayList* oldhead = nullptr;
	DelayList* newhead = delay_head;
	for (;
		newhead != nullptr;
		oldhead = newhead,
		newhead = newhead->next
		)
	{
		if(newhead->timeout > timeout)
		{
			break;
		}
	}
	if (delay_head == newhead)
	{
		ptr->next=newhead;
		delay_head = ptr;
	}
	else
	{
		ptr->next = newhead;
		oldhead->next = ptr;
	}
	EXIT_ATOMIC();
	bezbios_sched_free_cpu(0);
}
