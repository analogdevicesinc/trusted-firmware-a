/**
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright 2022, Analog Devices, Inc.
 */

#include <assert.h>
#include <stdio.h>

#include <bl31/bl31.h>
#include <bl31/ehf.h>
#include <drivers/arm/gicv3.h>
#include <drivers/console.h>
#include <drivers/generic_delay_timer.h>
#include <lib/mmio.h>
#include <lib/xlat_tables/xlat_tables_v2.h>
#include <plat/common/platform.h>

#include <platform_def.h>
#include <adi_uart4_console.h>

// @todo this forward declaration is no good--see about moving gic disable to uboot
void gicv3_rdistif_mark_core_asleep(uintptr_t gicr_base);

/**
 * @todo list:
 * - Need trustzone access controller configuration
 */

unsigned int plat_gic_mpidr_to_core_pos(unsigned long mpidr);

static console_t console;

static const mmap_region_t sc598_mmap[] = {
	MAP_REGION_FLAT(ADSP_SC598_GICD_BASE, ADSP_SC598_GIC_SIZE,
		MT_DEVICE | MT_RW | MT_SECURE),
	MAP_REGION_FLAT(ADSP_SC598_UART0_BASE, ADSP_SC598_UART_SIZE,
		MT_DEVICE | MT_RW | MT_SECURE),
	{0},
};

// Initialized in gicv3 init, but we need to allocate space for it
uintptr_t rdistif_base_addrs[PLATFORM_CORE_COUNT];

/**
 * @todo if we need any secure interrupts in el3, configure them with
 * interrupt_props below
 */
static const gicv3_driver_data_t gic_data = {
	.gicd_base = ADSP_SC598_GICD_BASE,
	.gicr_base = ADSP_SC598_GICR_BASE,
	.interrupt_props = NULL,
	.interrupt_props_num = 0,
	.rdistif_num = PLATFORM_CORE_COUNT,
	.rdistif_base_addrs = rdistif_base_addrs,
	.mpidr_to_core_pos = plat_gic_mpidr_to_core_pos,
};

static entry_point_info_t bl32_image_ep_info;
static entry_point_info_t bl33_image_ep_info;

ehf_pri_desc_t exception_priorities[] = {
//#if SDEI_SUPPORT
//	/* Critical priority SDEI */
//	EHF_PRI_DESC(PLAT_PRI_BITS, PLAT_SDEI_CRITICAL_PRI),
//
//	/* Normal priority SDEI */
//	EHF_PRI_DESC(PLAT_PRI_BITS, PLAT_SDEI_NORMAL_PRI),
//#endif
};

// @todo 3 bits for exception priorities?
EHF_REGISTER_PRIORITIES(exception_priorities, ARRAY_SIZE(exception_priorities),
	3);

/**
 * Only one core, hence always primary
 */
int plat_is_my_cpu_primary(void) {
	return 1;
}

/**
 * Always core 0
 */
unsigned int plat_my_core_pos(void) {
	return 0;
}

/**
 * Ignore mpidr, we're always core 0
 */
int plat_core_pos_by_mpidr(u_register_t mpidr) {
	return 0;
}
unsigned int plat_gic_mpidr_to_core_pos(unsigned long mpidr) {
	return plat_core_pos_by_mpidr(mpidr);
}

/**
 * @todo review PSCI integration and provide basic support at some point
 */
const unsigned char power_domain_tree[] = {
	PLATFORM_CLUSTER_COUNT,
	PLATFORM_CLUSTER0_CORE_COUNT,
};

const unsigned char *plat_get_power_domain_tree_desc(void) {
	return power_domain_tree;
}

void bl31_early_platform_setup2(u_register_t arg0, u_register_t arg1,
	u_register_t arg2, u_register_t arg3)
{
	// MMU must be enabled for bootrom to load the image; however it is assumed
	// disabled upon entry to BL31 so we disable it here rather than in SPL
	write_sctlr_el3(read_sctlr_el3() & ~(SCTLR_M_BIT | SCTLR_C_BIT | SCTLR_I_BIT));
	isb();
	dsb();

	// SPL also configured the GIC and we need to turn that back off
	gicv3_rdistif_mark_core_asleep(ADSP_SC598_GICR_BASE);

	console_adi_uart4_register(ADSP_SC598_UART0_BASE, &console);

	// @todo for falcon boot we will need to receive kernel arguments here
	bl33_image_ep_info.pc = BL33_BASE;
	bl33_image_ep_info.spsr = SPSR_64(MODE_EL1, MODE_SP_ELX, DISABLE_ALL_EXCEPTIONS);
	SET_SECURITY_STATE(bl33_image_ep_info.h.attr, NON_SECURE);

#ifdef SPD_opteed
	SET_PARAM_HEAD(&bl32_image_ep_info, PARAM_EP, VERSION_1, 0);
	SET_SECURITY_STATE(bl32_image_ep_info.h.attr, SECURE);
	bl32_image_ep_info.pc = BL32_BASE;
	bl32_image_ep_info.spsr = 0;
#endif

	// Enable coresight timer
	mmio_write_32(ADSP_SC598_TIMESTAMP_BASE, 1);

	// disable secure-only access to sharc cores so we can load from EL1
	mmio_write_32(ADSP_SC598_SPU0_SECUREC0, 0);
	mmio_write_32(ADSP_SC598_SPU0_SECUREC1, 0);
	mmio_write_32(ADSP_SC598_SPU0_SECUREC2, 0);

	// @todo tzasc configuration here if appropriate
}

void bl31_plat_arch_setup(void) {
	mmap_add_region(BL31_BASE, BL31_BASE, (BL31_LIMIT - BL31_BASE),
		MT_MEMORY | MT_RW | MT_SECURE);
	mmap_add_region(BL_CODE_BASE, BL_CODE_BASE, (BL_CODE_END - BL_CODE_BASE),
		MT_MEMORY | MT_RO | MT_SECURE | MT_EXECUTE);
	mmap_add(sc598_mmap);
	init_xlat_tables();
	enable_mmu_el3(0);
}

void bl31_platform_setup(void) {
	generic_delay_timer_init();
	gicv3_driver_init(&gic_data);
	gicv3_distif_init();
	gicv3_rdistif_init(0);
	gicv3_cpuif_enable(0);
}

entry_point_info_t *bl31_plat_get_next_image_ep_info(unsigned int type) {
	switch (type) {
	case NON_SECURE:
		return &bl33_image_ep_info;
	case SECURE:
		return &bl32_image_ep_info;
	default:
		return NULL;
	}
}

unsigned int plat_get_syscnt_freq2(void)
{
	return COUNTER_FREQUENCY;
}
