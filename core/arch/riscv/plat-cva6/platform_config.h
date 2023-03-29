/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright 2022-2023 NXP
 *
 * Brief   Qemu Virt platform configuration.
 */

#ifndef PLATFORM_CONFIG_H
#define PLATFORM_CONFIG_H

#include <mm/generic_ram_layout.h>

/* The stack pointer is always kept 16-byte aligned */
#define STACK_ALIGNMENT		16

/* DRAM */
#ifndef DRAM_BASE
#define DRAM_BASE		0x80000000
#define DRAM_SIZE		0x10000000
#endif

/* CLINT */
#ifndef CLINT_BASE
#define CLINT_BASE		0x02000000
#endif

/* PLIC */
#ifndef PLIC_BASE
#define PLIC_BASE		0x0c000000
#define PLIC_REG_SIZE		0x1000
#define PLIC_NUM_SOURCES	0x7f
#endif

/* UART */
#ifndef UART0_BASE
#define UART0_BASE		0x10000000
#endif
#define UART0_IRQ		0x0a

/* RTC */
#ifndef RTC_BASE
#define RTC_BASE		0x101000
#endif
#define RTC_IRQ			0x0b

#ifdef CFG_RISCV_MTIME_RATE
#define RISCV_MTIME_RATE CFG_RISCV_MTIME_RATE
#else
#define RISCV_MTIME_RATE 1000000
#endif

#endif /*PLATFORM_CONFIG_H*/
