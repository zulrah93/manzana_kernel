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
#endif