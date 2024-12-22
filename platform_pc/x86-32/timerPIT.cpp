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


struct DelayList :public List<DelayList> {
	volatile long timeout;
	ThreadControlBlock * tcb;
	DelayList() : List(false){};
	DelayList(DelayList * block) : List(block,false){};
};

static DelayList *  delay_head = nullptr;

long bezbios_get_ms()
{
	ENTER_ATOMIC();
	long ret = systick_msf >> 32;
	EXIT_ATOMIC();
	return ret;
}

static void PIT_IRQ(Isr_stack *)
{
	systick_msf +=systick_add;

	long now = systick_msf >> 32;
	while(delay_head != nullptr)
	{
		DelayList* newhead = delay_head;
		if(newhead->timeout > now)
			break;
		bezbios_sched_task_ready(newhead->tcb,1);
		delay_head = newhead->next;
		newhead->unplug();
	}


	bezbios_int_ack(0);

	if(!bezbios_sched_is_idle()) // if in main/idle task we don't do preemption
	{
		bezbios_sched_free_cpu(1); //Round-Robin
	}
}

void init_PIT() {
	outb(0x43,0x34);
	outb(0x40,PIT_RELOAD_INT &0xff);
	outb(0x40,PIT_RELOAD_INT >> 8);
	register_isr(0,PIT_IRQ);
	bezbios_enable_irq(0);
}

static void timer_head_exit(ThreadControlBlock * tid)
{
	for (DelayList * head=delay_head;head!=nullptr;head=head->next)
	{
		if (head->tcb == tid)
			head->unplug();
	}
}


void bezbios_delay_ms(int ms)
{
	ENTER_ATOMIC();
	static Exit_func efunc;
	efunc.init(timer_head_exit);

	long now = bezbios_get_ms();
	long timeout = now + ms;


	DelayList wait;

	DelayList* newhead = delay_head;
	DelayList* oldhead = nullptr;
	while(newhead != nullptr)
	{
		if(newhead->timeout > timeout)
			break;
		oldhead = newhead;
		newhead=newhead->next;
	}
	if(delay_head == newhead)
	{
		newhead->plug(&wait, true);
		delay_head = &wait;
	}
	else
	{
		wait.plug(oldhead, true);
	}


	wait.timeout = timeout;
	wait.tcb = bezbios_sched_get_tid();
	EXIT_ATOMIC();

	bezbios_sched_free_cpu(0);
}
