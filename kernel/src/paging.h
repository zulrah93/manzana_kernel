#ifndef PAGING_H
#define PAGIN_H

#include <stdint.h>

//Source: https://developer.arm.com/documentation/100403/0201/register-descriptions/aarch64-system-registers/ttbr1-el1--translation-table-base-register-1--el1
typedef struct {
    uint8_t common_not_private_supported : 1;
    uint64_t base_address : 48;
    uint16_t asid : 15;
} aarch64_translation_table_base_register_1_t;


uint64_t get_translation_table_base_register_1_el1() {
    uint64_t register_value;
    asm("MRS %x[data], TTBR1_EL1" : [data] "=r" (register_value));
    return register_value;
} 

aarch64_translation_table_base_register_1_t get_translation_table_base_register_1_el1_decoded() {
    uint64_t register_value = get_translation_table_base_register_1_el1();
    aarch64_translation_table_base_register_1_t* decoded_value = (aarch64_translation_table_base_register_1_t*)&register_value;
    return *decoded_value;
}

#endif