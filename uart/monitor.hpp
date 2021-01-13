/*
 * monitor.hpp
 *
 *  Created on: Jan 12, 2021
 *      Author: Korneliusz Osmenda
 */

#ifndef UART_MONITOR_HPP_
#define UART_MONITOR_HPP_


struct MonitorFunctions
{
	unsigned char funid;
	enum struct Type
	{
		TERMINATOR = 0,
		ARRAY_ARGUMENTS,
	};
	Type type;
	union {
	long (*array_arguments)(unsigned char params[],long len);
	} callback;
};

extern const MonitorFunctions monitor_functions[];






#endif /* UART_MONITOR_HPP_ */
