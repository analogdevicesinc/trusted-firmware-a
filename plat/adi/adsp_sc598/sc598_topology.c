/**
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright 2026, Analog Devices, Inc.
 */

#include <arch.h>
#include <plat/common/platform.h>

#include <platform_def.h>

static const unsigned char plat_power_domain_tree_desc[] = {
	PLATFORM_SYSTEM_COUNT,
	PLATFORM_CLUSTER_COUNT,
	PLATFORM_CPUS_PER_CLUSTER,
};

const unsigned char *plat_get_power_domain_tree_desc(void)
{
	return plat_power_domain_tree_desc;
}

 /*
 * Always core 0
 */
unsigned int plat_my_core_pos(void) {
	return 0;
}

int plat_core_pos_by_mpidr(u_register_t mpidr)
{
	unsigned int cluster = MPIDR_AFFLVL1_VAL(mpidr);
	unsigned int core = MPIDR_AFFLVL0_VAL(mpidr);

	if (MPIDR_AFFLVL3_VAL(mpidr) > 0 ||
	    MPIDR_AFFLVL2_VAL(mpidr) > 0 ||
	    cluster >= PLATFORM_CLUSTER_COUNT ||
	    core >= PLATFORM_CPUS_PER_CLUSTER) {
		return -1;
	}
	return (cluster * PLATFORM_CPUS_PER_CLUSTER) + core;
}
