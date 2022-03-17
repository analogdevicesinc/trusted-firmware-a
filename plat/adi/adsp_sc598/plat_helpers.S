/**
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2022, Analog Devices, Inc.
 */

#include <asm_macros.S>
#include <drivers/console.h>

.globl plat_crash_console_init
.globl plat_crash_console_putc
.globl plat_crash_console_flush

/**
 * Console is already initialized with crash mode
 */
func plat_crash_console_init
	mov x0, #1
	ret x3
endfunc plat_crash_console_init

func plat_crash_console_putc
	b console_putc
endfunc plat_crash_console_putc

func plat_crash_console_flush
	b console_flush
endfunc plat_crash_console_flush
