#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdint.h>

uint64_t get_system_ticks() {
    uint64_t register_value;
    asm("MRS %x[data], CNTPCT_EL0" : [data] "=r" (register_value));
    return register_value;
}

uint64_t get_counter_timer_frequency() {
    uint64_t register_value;
    asm("MRS %x[data], CNTFRQ_EL0" : [data] "=r" (register_value));
    return register_value;
}

uint32_t get_pmcr_el0() {
    uint32_t register_value;
    asm("MRS %x[data], PMCR_EL0" : [data] "=r" (register_value));
    return register_value;
}

uint64_t get_pmccntr_el0() {
    uint64_t register_value;
    asm("MRS %x[data], PMCCNTR_EL0" : [data] "=r" (register_value));
    return register_value;
}

uint64_t get_pmsuserenr_el0() {
    uint64_t register_value;
    asm("MRS %x[data], PMUSERENR_EL0" : [data] "=r" (register_value));
    return register_value;
}


uint64_t get_pmuacr_el1() {
    uint64_t register_value;
    asm("MRS %x[data], PMUACR_EL1" : [data] "=r" (register_value));
    return register_value;
}

//https://support.arm.com/documentation/100442/0100/debug-registers/aarch64-pmu-registers/pmcr-el0--performance-monitors-control-register--el0
typedef struct {
    uint8_t enable : 1;
    uint8_t event_counter_reset : 1;
    uint8_t clock_counter_reset : 1;
    uint8_t enable_clock_divider : 1;
    uint8_t export_enable : 1;
    uint8_t disable_cycle_counter : 1;
    uint8_t enable_long_cycle_count : 1;
    uint8_t reserved_1 : 4;
    uint8_t number_of_event_counters : 5;
    uint8_t id_code;
    uint8_t implementer_code;
} pmcr_el0_t;

pmcr_el0_t get_pmcr_el0_decoded() {
    uint32_t value = get_pmcr_el0();
    pmcr_el0_t* decoded = (pmcr_el0_t*)&value;
    return *decoded;
}

typedef struct {
    uint8_t enable : 1;
    uint8_t sw : 1;
    uint8_t cr : 1;
    uint8_t er : 1;
    uint8_t uen : 1;
    uint8_t ir : 1;
    uint8_t tid : 1;
    uint64_t reserved : 56;
} pmsuserenr_el0_t;

void set_pmsuserenr_el0(pmsuserenr_el0_t encoded_value) {
    uint64_t* decoded = (uint64_t*)&encoded_value;
    asm("MSR PMUSERENR_EL0, %x[data]" :: [data] "r" (*decoded) : "memory");
}

pmsuserenr_el0_t get_pmsuserenr_el0_decoded() {
    uint64_t value = get_pmsuserenr_el0();
    pmsuserenr_el0_t* decoded = (pmsuserenr_el0_t*)&value;
    return *decoded;
}

typedef struct {
    uint32_t p : 31;
    uint8_t c : 1;
    uint8_t fm : 1;
    uint32_t reserved : 30;
} pmuacr_el1_t;

void set_pmuacr_el1(pmuacr_el1_t encoded_value) {
    uint64_t* decoded = (uint64_t*)&encoded_value;
    asm("MSR PMUACR_EL1, %x[data]" :: [data] "r" (*decoded) : "memory");
}

pmuacr_el1_t get_pmuacr_el1_decoded() {
    uint64_t value = get_pmuacr_el1();
    pmuacr_el1_t* decoded = (pmuacr_el1_t*)&value;
    return *decoded;
}

uint64_t get_current_cpu_frequency() {
    pmsuserenr_el0_t pms_user = get_pmsuserenr_el0_decoded();
    pms_user.enable = 1;
    set_pmsuserenr_el0(pms_user);
    pmuacr_el1_t pmuac = get_pmuacr_el1_decoded();
    pmuac.c = 1;
    uint64_t pmc_counter = get_pmccntr_el0();
    const uint64_t inital_ticks = get_system_ticks();
    const uint64_t cycles_100ms = (get_counter_timer_frequency() / 1000) * 100;
    const uint64_t inital_pm_ticks = get_pmccntr_el0();
    uint64_t ticks;
    do {
        ticks = get_system_ticks();
    }
    while (ticks < (inital_ticks + cycles_100ms));
    return 0;
}


typedef struct {
    uint8_t reserved_1 : 2;
    uint8_t level : 2;
    uint64_t reserved_2 : 60;
} exception_level_t;

exception_level_t get_current_exception_level() {
    uint64_t encoded_el;
    asm("MRS %x[data], CurrentEl" : [data] "=r" (encoded_el));
    exception_level_t* el_info = (exception_level_t*)&encoded_el;
    return *el_info;
}

//Source: https://developer.arm.com/documentation/ddi0500/d/system-control/aarch64-register-descriptions/system-control-register--el1
uint64_t get_sctrl_el1() {
    uint64_t register_value;
    asm("MRS %x[data], SCTLR_EL1" : [data] "=r" (register_value));
    return register_value;
}

bool is_little_endian_system() {
    return 0 == (get_sctrl_el1() >> 7);
}

//Source: https://developer.arm.com/documentation/111107/2026-03/AArch64-Registers/ID-AA64PFR1-EL1--AArch64-Processor-Feature-Register-1
typedef struct {
    uint8_t feat_bt : 4;
    uint8_t feat_ssb : 4;
    uint8_t feat_mte : 4;
    uint8_t feat_ras : 4;
    uint8_t feat_mpam : 4;
    uint8_t feat_sme : 4;
    uint8_t feat_rng_trap : 4;
    uint8_t feat_csv2 : 4;
    uint8_t feat_nmi : 4;
    uint8_t feat_mte_frac : 4;
    uint8_t feat_gcs : 4;
    uint8_t feat_the : 4;
    uint8_t feat_mtex : 4;
    uint8_t feat_df2 : 4;
    uint8_t feat_pfar : 4;
} aarch64_processor_feature_t;

uint64_t get_id_aa64pfr1_el1_register() {
    uint64_t register_value;
    asm("MRS %x[data], ID_AA64PFR1_EL1" : [data] "=r" (register_value));
    return register_value;
}

aarch64_processor_feature_t get_id_aa64pfr1_el1_register_decoded() {
    uint64_t register_value = get_id_aa64pfr1_el1_register();
    aarch64_processor_feature_t* decoded_value = (aarch64_processor_feature_t*)&register_value;
    return *decoded_value;
}

typedef struct {
    uint8_t revision_id : 4;
    uint16_t part_number : 12;
    uint8_t architecture : 4;
    uint8_t variant : 4;
    uint8_t implementer_code;
    uint32_t reserved;
} aarch64_main_id_register_t;

uint64_t get_main_id_register() {
    uint64_t register_value;
    asm("MRS %x[data], MIDR_EL1" : [data] "=r" (register_value));
    return register_value;
}

aarch64_main_id_register_t get_main_id_register_decoded() {
    uint64_t register_value = get_main_id_register();
    aarch64_main_id_register_t* decoded_value = (aarch64_main_id_register_t*)&register_value;
    return *decoded_value;
}

typedef struct {
    uint8_t mmu_enabled : 1;
    uint8_t alignment_check_enabled : 1;
    uint8_t cache_enable : 1;
    uint8_t sp_align_check_enabled : 1;
    uint8_t el0_stack_align_check_enabled : 1;
    uint8_t cp15_barrier_enabled : 1;
    uint8_t reservered_0 : 1;
    uint8_t itd_instruction_disable : 1;
    uint8_t setend_instruction_disable : 1;
    uint8_t user_mask_access_enable : 1;
    uint8_t reserved_1 : 2;
    uint8_t instruction_cache_enable : 1;
    uint8_t reserved_3 : 1;
    uint8_t dc_zva_instructions_el0_enable : 1;
    uint8_t enable_el0_ctr_el0 : 1;
    uint8_t wfi_traps : 1;
    uint8_t reserved_4 : 1;
    uint8_t wfe_traps : 1;
    uint8_t write_executes_never : 1;
    uint8_t reserved_5 : 4;
    uint8_t data_access_el0_is_big_endian : 1;
    uint8_t exceptions_are_big_endian : 1;
    uint8_t enable_uci : 1;
    uint8_t reserved_6 : 5;
} aarch64_system_control_register_t;

uint32_t get_system_control_register() {
    uint32_t register_value;
    asm("MRS %x[data], SCTLR_EL1" : [data] "=r" (register_value));
    return register_value;
}

aarch64_system_control_register_t get_system_control_register_decoded() {
    uint32_t register_value = get_system_control_register();
    aarch64_system_control_register_t* decoded_value = (aarch64_system_control_register_t*)&register_value;
    return *decoded_value;
}


void set_system_control_register(aarch64_system_control_register_t scr_encoded) {
    uint32_t* scr_decoded = (uint32_t*)&scr_encoded;
    asm("MSR SCTLR_EL1, %x[data]" :: [data] "r" (*scr_decoded) : "memory");
}

typedef struct {
    uint8_t attributes[8];
} aarch64_memory_indirection_register_t;

uint64_t get_memory_indirection_register_el1() {
    uint64_t register_value;
    asm("MRS %x[data], MAIR_EL1" : [data] "=r" (register_value));
    return register_value;
}

aarch64_memory_indirection_register_t get_memory_indirection_register_el1_decoded() {
    uint64_t register_value = get_memory_indirection_register_el1();
    aarch64_memory_indirection_register_t* decoded_value = (aarch64_memory_indirection_register_t*)&register_value;
    return *decoded_value;
}

// Include the virtual memory management stuff 
#include <paging.h>

#endif