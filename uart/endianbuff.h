/*
 * endianbuff.h
 *
 *  Created on: Jan 7, 2021
 *      Author: Korneliusz Osmenda
 */

#ifndef UART_ENDIANBUFF_H_
#define UART_ENDIANBUFF_H_

static inline unsigned short get_short_le(const unsigned char *buff)
{
	unsigned short ret= buff[0];
	ret |= ((unsigned short)buff[1])<<8;
	return ret;
}

static inline unsigned long get_long_le(const unsigned char *buff)
{
	unsigned long ret= buff[0];
	ret |= ((unsigned long)buff[1])<<8;
	ret |= ((unsigned long)buff[2])<<16;
	ret |= ((unsigned long)buff[3])<<24;
	return ret;
}

static inline void put_short_le(unsigned char *buff, unsigned short val)
{
	buff[0] = val;
	buff[1] = val>>8;
}

#endif /* UART_ENDIANBUFF_H_ */
