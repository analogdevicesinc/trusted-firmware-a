##
 # SPDX-License-Identifier: BSD-3-Clause
 # Copyright (c) 2022, Analog Devices, Inc.
 ##

# Configuration options specific to hardware
COLD_BOOT_SINGLE_CPU := 1
HW_ASSISTED_COHERENCY := 1
RESET_TO_BL31 := 0
USE_COHERENT_MEM := 0
PROGRAMMABLE_RESET_ADDRESS := 0
ERRATA_A55_1530923 := 1
GICV3_SUPPORT_GIC600 := 1
EL3_EXCEPTION_HANDLING := 1
ENABLE_CONSOLE_GETC := 1
INIT_UNUSED_NS_EL2 := 1

# Debug configuration to change later
CRASH_REPORTING := 1

# Build file specifications
PLAT_INCLUDES := -Iplat/adi/adsp_sc598 \
                 -Iplat/adi

include drivers/arm/gic/v3/gicv3.mk
include lib/xlat_tables_v2/xlat_tables.mk

# Bootrom -> SPL -> BL31 -> BL32 -> uboot/kernel
# All verification done in SPL, use none of the trusted boot support

BL31_SOURCES += common/desc_image_load.c \
                drivers/delay_timer/delay_timer.c \
                drivers/delay_timer/generic_delay_timer.c \
                ${GICV3_SOURCES} \
                lib/cpus/aarch64/cortex_a55.S \
                lib/optee/optee_utils.c \
                plat/adi/adi_uart4_console.S \
                plat/adi/adsp_sc598/plat_helpers.S \
                plat/adi/adsp_sc598/psci.c \
                plat/adi/adsp_sc598/sc598_bl31.c \
                plat/common/plat_gicv3.c \
                plat/common/plat_psci_common.c \
                ${XLAT_TABLES_LIB_SRCS}
