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

void bezbios_serial_init();
void bezbios_serial_send(unsigned char byte);
unsigned char bezbios_serial_recv();

#define BEZBIOS_INIT_SERIAL() \
		__attribute((constructor)) \
		static \
		void SERIAL_init() \
		{ \
			bezbios_serial_init(); \
		}

#ifdef __cplusplus
}
#endif

#endif /* SCHED_BEZBIOS_SCHED_API_H_ */
