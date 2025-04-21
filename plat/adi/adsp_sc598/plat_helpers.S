/**
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2022, Analog Devices, Inc.
 */

#include <asm_macros.S>
#include <drivers/console.h>
#include <platform_def.h>

#define UART_STATUS 0x08
#define UART_RBR 0x20
#define UART_THR 0x24

.globl plat_crash_console_init
.globl plat_crash_console_putc
.globl plat_crash_console_flush

/**
 * Console is already initialized with crash mode
 */
func plat_crash_console_init
	mov x0, #1
	ret
endfunc plat_crash_console_init

func plat_crash_console_putc
	ldr x1, =ADSP_SC598_UART0_BASE

	/* If \n, send \r first */
	cmp w0, #0x0a
	b.ne	wait

wait_cr:
	ldr w2, [x1, #UART_STATUS]
	tbz w2, #5, wait_cr
	mov w2, #0x0d
	str w2, [x1, #UART_THR]

wait:
	ldr w2, [x1, #UART_STATUS]
	tbz w2, #5, wait

	str w0, [x1, #UART_THR]
	ret
endfunc plat_crash_console_putc

func plat_crash_console_flush
	ret
endfunc plat_crash_console_flush
