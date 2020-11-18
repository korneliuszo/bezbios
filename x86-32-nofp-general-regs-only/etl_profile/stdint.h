/*
 * stdint.h
 *
 *  Created on: Nov 18, 2020
 *      Author: Korneliusz Osmenda
 */

#ifndef X86_32_NOFP_GENERAL_REGS_ONLY_ETL_PROFILE_STDINT_H_
#define X86_32_NOFP_GENERAL_REGS_ONLY_ETL_PROFILE_STDINT_H_

#define UINT8_MAX		(255)
#define UINT16_MAX		(65535)
#define UINT32_MAX		(4294967295U)
#define UINTPTR_MAX		(4294967295U)

typedef long size_t;
typedef long ptrdiff_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef long int32_t;
typedef unsigned long uint32_t;


#endif /* X86_32_NOFP_GENERAL_REGS_ONLY_ETL_PROFILE_STDINT_H_ */
