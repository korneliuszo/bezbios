/*
 * bezbios_uart_api_low.h
 *
 *  Created on: Dec 30, 2020
 *      Author: Korneliusz Osmenda
 */

#ifndef UART_BEZBIOS_UART_API_LOW_H_
#define UART_BEZBIOS_UART_API_LOW_H_

#ifdef __cplusplus
extern "C"
{
#endif

void bezbios_serial_bootup_init();
void bezbios_low_write_serial(char a);
void bezbios_low_wait();

#ifdef __cplusplus
}
#endif



#endif /* UART_BEZBIOS_UART_API_LOW_H_ */
