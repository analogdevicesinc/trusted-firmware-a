/**
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c), 2026, Analog Devices, Inc.
 */
#ifndef ADI_UART4_CONSOLE_H
#define ADI_UART4_CONSOLE_H

#include <drivers/console.h>
#include <stdint.h>

int console_adi_uart4_register(uintptr_t, console_t *);

#endif
