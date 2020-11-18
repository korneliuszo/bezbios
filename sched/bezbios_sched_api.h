/*
 * bezbios_sched_api.h
 *
 *  Created on: Nov 11, 2020
 *      Author: Korneliusz Osmenda
 */

#ifndef SCHED_BEZBIOS_SCHED_API_H_
#define SCHED_BEZBIOS_SCHED_API_H_

#define CONFIG_MAX_THREADS 15

#ifdef __cplusplus
extern "C"
{
#endif

void bezbios_sched_switch_context(int nexttask);
int bezbios_sched_get_tid();
int bezbios_sched_create_task(void(*entry)(),void * stackbottom);
void bezbios_sched_destroy_task(int tid);

#ifdef __cplusplus
}
#endif

#endif /* SCHED_BEZBIOS_SCHED_API_H_ */
