#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdint.h>

uint64_t get_system_ticks() {
    uint64_t register_value;
    asm("MRS %x[data], CNTPCT_EL0" : [data] "=r" (register_value));
    return register_value;
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

#endif