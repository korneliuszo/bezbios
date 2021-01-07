/*
 * monitor.cpp
 *
 *  Created on: Jan 6, 2021
 *      Author: Korneliusz Osmenda
 */

#include "tlay2.hpp"



void tlay2_monitor(Tlay2 & tlay2)
{
	switch(tlay2.payload[0])
	{
	case 0x00: //PING
		tlay2.reply(tlay2.payload,tlay2.len);
		break;
	}
}
