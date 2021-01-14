/*
 * tlay2_uart.cpp
 *
 *  Created on: Jan 5, 2021
 *      Author: Korneliusz Osmenda
 */

#include "tlay2.hpp"
#include "bezbios_uart_api.h"

Tlay2 tlay2_uart(bezbios_serial_send,bezbios_serial_recv);

static void tlay2_uart_entry()
{
	tlay2_uart.process();
}

static BEZBIOS_CREATE_PROCESS<tlay2_uart_entry,4096> tlay2_uart_task;
