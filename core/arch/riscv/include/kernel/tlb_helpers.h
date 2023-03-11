/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright 2022 NXP
 */

#ifndef TLB_HELPERS_H
#define TLB_HELPERS_H

#ifndef __ASSEMBLER__

void tlbi_all(void);
void tlbi_asid(unsigned long asid);
void tlbi_mva_allasid(unsigned long addr);
static inline void tlbi_mva_allasid_nosync(unsigned long va) {
    tlbi_mva_allasid(va);
}
void tlbi_mva_asid(unsigned long va, uint32_t asid);
static inline void tlbi_mva_asid_nosync(unsigned long va, uint32_t asid) {
    tlbi_mva_asid(va, asid);
}

#endif /*!__ASSEMBLER__*/

#endif /* TLB_HELPERS_H */
