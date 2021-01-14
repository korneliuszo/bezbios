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

#ifdef __cplusplus
}

class BEZBIOS_INIT_SERIAL{
public:
	BEZBIOS_INIT_SERIAL()
	{
		bezbios_serial_init();
	}
};

#endif

#endif /* SCHED_BEZBIOS_SCHED_API_H_ */
