/*
 * interrupts_default.cpp
 *
 *  Created on: Aug 31, 2021
 *      Author: Korneliusz Osmenda
 */

#include "interrupts.h"

template<unsigned char IRQ>
__attribute__((weak))
__attribute__((cdecl))
void bezbios_imp_hw_req<IRQ>::f(Isr_stack *)
    {
    	bezbios_int_ack(IRQ); //eat spurious interrupts
    }

template class bezbios_imp_hw_req<0>;
template class bezbios_imp_hw_req<1>;
template class bezbios_imp_hw_req<2>;
template class bezbios_imp_hw_req<3>;
template class bezbios_imp_hw_req<4>;
template class bezbios_imp_hw_req<5>;
template class bezbios_imp_hw_req<6>;
template class bezbios_imp_hw_req<7>;
template class bezbios_imp_hw_req<8>;
template class bezbios_imp_hw_req<9>;
template class bezbios_imp_hw_req<10>;
template class bezbios_imp_hw_req<11>;
template class bezbios_imp_hw_req<12>;
template class bezbios_imp_hw_req<13>;
template class bezbios_imp_hw_req<14>;
template class bezbios_imp_hw_req<15>;
