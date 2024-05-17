// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright 2022 NXP
 */

#include <riscv.h>
#include <sbi.h>
#include <sm/optee_sbi.h>
#include <trace.h>
#include <teesmc_opteed.h>

struct sbiret {
	long error;
	long value;
};

#define _sbi_ecall(ext, fid, arg0, arg1, arg2, arg3, arg4, arg5, ...) ({  \
	register unsigned long a0 asm("a0") = (unsigned long)arg0; \
	register unsigned long a1 asm("a1") = (unsigned long)arg1; \
	register unsigned long a2 asm("a2") = (unsigned long)arg2; \
	register unsigned long a3 asm("a3") = (unsigned long)arg3; \
	register unsigned long a4 asm("a4") = (unsigned long)arg4; \
	register unsigned long a5 asm("a5") = (unsigned long)arg5; \
	register unsigned long a6 asm("a6") = (unsigned long)fid;  \
	register unsigned long a7 asm("a7") = (unsigned long)ext;  \
	asm volatile ("ecall" \
		: "+r" (a0), "+r" (a1), "+r" (a2) \
		: "r" (a3), "r" (a4), "r" (a5), "r"(a6), "r"(a7) \
		: "memory"); \
	(struct sbiret){ .error = a0, .value = a1 }; \
})

#define sbi_ecall(...) _sbi_ecall(__VA_ARGS__, 0, 0, 0, 0, 0, 0, 0)

void sbi_console_putchar(int ch)
{
	sbi_ecall(SBI_EXT_0_1_CONSOLE_PUTCHAR, (unsigned long)ch);
}

int sbi_boot_hart(uint32_t hart_id, paddr_t start_addr, unsigned long arg)
{
	struct sbiret ret;

	ret = sbi_ecall(SBI_EXT_HSM, SBI_EXT_HSM_HART_START, hart_id, start_addr, arg);

	return ret.error;
}

#define SBI_EXTID_TEE (0x544545)


#include <tee/entry_fast.h>
#include <sm/optee_sbi.h>

static void sbi_tz_ecall(unsigned long ext, unsigned long fid, 
			unsigned long arg0, unsigned long arg1, 
			unsigned long arg2, unsigned long arg3, 
			unsigned long arg4, unsigned long arg5,
			struct thread_smc_args *res) {
	register unsigned long a0 asm("a0") = (unsigned long)arg0;
	register unsigned long a1 asm("a1") = (unsigned long)arg1;
	register unsigned long a2 asm("a2") = (unsigned long)arg2;
	register unsigned long a3 asm("a3") = (unsigned long)arg3;
	register unsigned long a4 asm("a4") = (unsigned long)arg4;
	register unsigned long a5 asm("a5") = (unsigned long)arg5;
	register unsigned long a6 asm("a6") = (unsigned long)fid;
	register unsigned long a7 asm("a7") = (unsigned long)ext;

    asm volatile("ecall"
                 : "=r"(a0), "=r"(a1), "=r"(a2), "=r"(a3), "=r"(a4), "=r"(a5), "=r"(a6), "=r"(a7)
                 : "r"(a0), "r"(a1), "r"(a2), "r"(a3), "r"(a4), "r"(a5), "r"(a6), "r"(a7)
                 : "memory");

    if (res != NULL)
    {
	res->a0 = a0;
	res->a1 = a1;
	res->a2 = a2;
	res->a3 = a3;
	res->a4 = a4;
	res->a5 = a5;
	res->a6 = a6;
	res->a7 = a7;
    }
    /* unsigned long val; */
    /* asm volatile ("rdtime %0": "=r"(val)); */
    /* EMSG("after  %lu\n", val); */
}

void thread_handle_fast_smc(struct thread_smc_args *args);
uint32_t thread_handle_std_smc(uint32_t a0, uint32_t a1, uint32_t a2,
			       uint32_t a3, uint32_t a4, uint32_t a5,
			       uint32_t a6 __unused, uint32_t a7 __maybe_unused);
void bao_return_to_ree(unsigned long a0, unsigned long a1,
			unsigned long a2, unsigned long a3,
			unsigned long a4, unsigned long a5,
			unsigned long a6, unsigned long a7);

void __attribute__((__noreturn__)) crossconhyp_return_to_ree(unsigned long a0, unsigned long a1,
			unsigned long a2, unsigned long a3,
			unsigned long a4, unsigned long a5,
			unsigned long a6, unsigned long a7)
{
    struct thread_smc_args res;
    unsigned long ret;

    while (1){
	sbi_tz_ecall(SBI_EXTID_TEE, a0,
		a0, a1, a2, a3, a4, a5, &res);
	if(OPTEE_SMC_IS_FAST_CALL(res.a0)){
	    thread_handle_fast_smc(&res);
	    a0 = TEESMC_OPTEED_RETURN_CALL_DONE;
	    a1 = res.a0;
	    a2 = res.a1;
	    a3 = res.a2;
	    a4 = res.a3;
	    a5 = res.a4;
	    a6 = res.a5;
	    a7 = res.a6;
	}else{
	    // Handle the return from the SMC call
            ret = thread_handle_std_smc(res.a0, res.a1, res.a2, res.a3,
                    res.a4, res.a5, res.a6, res.a7);
            a0 = ret;
	}
    }
}

void mu_service() {
    crossconhyp_return_to_ree(TEESMC_OPTEED_FUNCID_RETURN_ENTRY_DONE, 0, 0, 0, 0, 0, 0, 0);
    while(1);
}
