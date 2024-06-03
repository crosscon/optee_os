$(call force,CFG_RV64_core,y)

$(call force,CFG_CORE_LARGE_PHYS_ADDR,y)
$(call force,CFG_TEE_CORE_DEBUG,n)
$(call force,CFG_CORE_DYN_SHM,n)

# Crypto flags
$(call force,CFG_WITH_SOFTWARE_PRNG,y)

# Protection flags
$(call force,CFG_CORE_ASLR,n)
$(call force,CFG_WITH_STACK_CANARIES,n)
$(call force,CFG_CORE_SANITIZE_KADDRESS,n)

# Hart-related flags
$(call force,CFG_TEE_CORE_NB_CORE,1)
$(call force,CFG_NUM_THREADS,1)
$(call force,CFG_BOOT_SYNC_CPU,y)

# RISC-V-specific flags
$(call force,CFG_RISCV_PLIC,y)
$(call force,CFG_SBI_CONSOLE,n)
$(call force,CFG_16550_UART,y)
$(call force,CFG_RISCV_TIME_SOURCE_RDTIME,y)
CFG_RISCV_MTIME_RATE ?= 10000000

# TA-related flags
supported-ta-targets = ta_rv64

# Memory layout flags
CFG_TDDRAM_START ?= 0x80200000
CFG_TDDRAM_SIZE  ?= 0x00f00000
$(call force,CFG_CORE_RESERVED_SHM,y)
CFG_SHMEM_START  ?= 0x89100000
CFG_SHMEM_SIZE   ?= 0x00200000
CFG_TEE_RAM_VA_SIZE ?= 0x00200000

$(call force,CFG_RISCV_M_MODE,n)
$(call force,CFG_RISCV_S_MODE,y)
$(call force,CFG_RISCV_SBI,y)
$(call force,CFG_TEE_CORE_EMBED_INTERNAL_TESTS,y)
$(call force,CFG_WITH_LPAE,y)

$(call force,CFG_ATTESTATION_PTA,y)
$(call force,CFG_DEVICE_ENUM_PTA,y)
$(call force,CFG_SECSTOR_TA_MGMT_PTA,y)
$(call force,CFG_SYSTEM_PTA,y)
$(call force,CFG_RTC_PTA,y)

$(call force,CFG_APDU_PTA,n)
$(call force,CFG_SCP03_PTA,n)
$(call force,CFG_SCMI_PTA,n)
$(call force,CFG_HWRNG_PTA,n)

$(call force,CFG_TA_ASLR,n)
$(call force,CFG_TA_FLOAT_SUPPORT,n)




