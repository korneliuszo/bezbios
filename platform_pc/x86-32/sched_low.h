/*
 * sched_low.h
 *
 *  Created on: Nov 15, 2020
 *      Author: Korneliusz Osmenda
 */

#ifndef SCHED_X86_32_NOFP_GENERAL_REGS_ONLY_GCC8_2_0_M32_KMODE_SCHED_LOW_H_
#define SCHED_X86_32_NOFP_GENERAL_REGS_ONLY_GCC8_2_0_M32_KMODE_SCHED_LOW_H_

namespace BezBios {
namespace Sched {
namespace m32ngro {

void switchcontext_int(void * volatile *prev_s, void *next_s, void (*entry_s)(void *),void * val_s);
}
}
}


#endif /* SCHED_X86_32_NOFP_GENERAL_REGS_ONLY_GCC8_2_0_M32_KMODE_SCHED_LOW_H_ */
