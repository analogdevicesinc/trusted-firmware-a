/**
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (c) 2022, Analog Devices, Inc.
 */

#include <arch.h>
#include <lib/mmio.h>
#include <lib/psci/psci.h>

/*
 * Reboot the system by asserting sysreset which will lead to a reboot
 */
void __dead2 adsp_sc598_rcu_reset(void) {
	uint32_t val = mmio_read_32(ADSP_SC598_RCU_BASE + ADI_RCU_REG_CTL);
	mmio_write_32(ADSP_SC598_RCU_BASE + ADI_RCU_REG_CTL, val | ADI_RCU_CTL_SYSRST);

	while (1)
		;
}

/*
 * Turn off by putting the Arm core into reset
 */
void __dead2 adsp_sc598_rcu_off(void) {
	uint32_t val = mmio_read_32(ADSP_SC598_RCU_BASE + ADI_RCU_REG_CRCTL);
	mmio_write_32(ADSP_SC598_RCU_BASE + ADI_RCU_REG_CRCTL, val | 1);

	while (1)
		;
}

void plat_soft_reset(void){
	adsp_sc598_rcu_reset();

	while(1)
		;
}

static const plat_psci_ops_t adsp_sc598_psci_ops = {
	.system_off = adsp_sc598_rcu_off,
	.system_reset = adsp_sc598_rcu_reset,
};

/*
 * PSCI integration is intentionally minimal for the ADSP-SC598.
 * Only system_off and system_reset are provided since PLATFORM_CORE_COUNT=1.
 * CPU hotplug, suspend/resume, and power domain management are not supported.
 * These will be implemented in future products when multi-core or
 * power management features are enabled.
 */
int plat_setup_psci_ops(uintptr_t sec_entrypoint, const plat_psci_ops_t **psci_ops) {
	*psci_ops = &adsp_sc598_psci_ops;
	return 0;
}
