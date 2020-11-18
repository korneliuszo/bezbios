/*
 * bezbios_sched_api.h
 *
 *  Created on: Nov 11, 2020
 *      Author: Korneliusz Osmenda
 */

#ifndef SCHED_BEZBIOS_UART_API_H_
#define SCHED_BEZBIOS_UART_API_H_

#ifdef __cplusplus
extern "C"
{
#endif

char bezbios_low_read_serial();
void bezbios_low_write_serial(char a);

#ifdef __cplusplus
}
#endif

#endif /* SCHED_BEZBIOS_SCHED_API_H_ */
