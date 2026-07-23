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

uint8_t get_current_exception_level() {
    uint64_t encoded_el;
    asm("MRS %x[data], CurrentEl" : [data] "=r" (encoded_el));
    exception_level_t* el_info = (exception_level_t*)&encoded_el;
    return el_info->level;
}

#endif