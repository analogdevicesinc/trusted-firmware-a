/**
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2022, Analog Devices, Inc.
 */

#include <arch.h>
#include <asm_macros.S>
#include <console_macros.S>
#include <assert_macros.S>

#define ADI_UART4_STATUS 0x08

/* bit positions not numbers */
#define ADI_UART4_STATUS_THRE 5
#define ADI_UART4_STATUS_DR 0

#define ADI_UART4_RBR 0x20
#define ADI_UART4_THR 0x24

.globl console_adi_uart4_register
.globl console_adi_uart4_putc
.globl console_adi_uart4_getc
.globl console_adi_uart4_flush

/*
 * base address, pointer to console struct
 * Most configuration is inherited from SPL setup
 */
func console_adi_uart4_register
	cbz x1, register_fail
	cbz x0, register_fail
	str x0, [x1, #CONSOLE_T_BASE]
	mov x0, x1
	finish_console_register adi_uart4 putc=1, getc=1, flush=1

register_fail:
	mov w0, #-1
	ret x30
endfunc console_adi_uart4_register

/* THRE is bit 5 */
func console_adi_uart4_putc
	ldr x1, [x1, #CONSOLE_T_BASE]
	cbz x1, putc_error

	/* If \n, send \r first */
	cmp w0, #0x0a
	b.ne	wait

wait_cr:
	ldr w2, [x1, #ADI_UART4_STATUS]
	tbz w2, #5, wait_cr
	mov w2, #0x0d
	str w2, [x1, #ADI_UART4_THR]

wait:
	ldr w2, [x1, #ADI_UART4_STATUS]
	tbz w2, #5, wait

	str w0, [x1, #ADI_UART4_THR]
	ret

putc_error:
	mov w0, #-1
	ret
endfunc console_adi_uart4_putc

/* DR is bit 0 */
func console_adi_uart4_getc
	cbz x0, getc_error
	ldr x0, [x0, #CONSOLE_T_BASE]

read_wait:
	ldr w1, [x0, #ADI_UART4_STATUS]
	tbz w1, #0, read_wait

	ldr w1, [x0, #ADI_UART4_RBR]

	/* clear status register */
	mov x2, #-1
	str x2, [x0, #ADI_UART4_STATUS]

	mov w0, w1
	ret

getc_error:
	mov w0, #-1
	ret
endfunc console_adi_uart4_getc

func console_adi_uart4_flush
	ret
endfunc console_adi_uart4_flush
