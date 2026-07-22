#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdint.h>

uint64_t get_system_ticks() {
    uint64_t register_value;
    asm("MRS %x[data], CNTPCT_EL0" : [data] "=r" (register_value));
    return register_value;
}

#endif