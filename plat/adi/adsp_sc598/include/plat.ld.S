/*
 * Copyright (c) 2026, Analog Devices, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef ADI_SC598_PLAT_LD_S
#define ADI_SC598_PLAT_LD_S

/* Redefine DATA_SECTION with end alignment for SC598 */
#undef DATA_SECTION
#define DATA_SECTION					\
	.data . : ALIGN(DATA_ALIGN) {			\
		__DATA_START__ = .;			\
		*(SORT_BY_ALIGNMENT(.data*))		\
		. = ALIGN(DATA_ALIGN);			\
		__DATA_END__ = .;			\
	}

#endif /* ADI_SC598_PLAT_LD_S */
