#ifndef RANDOM_H
#define RANDOM_H

// The following functions define a portable implementation of rand and srand.

static uint32_t next = 1;  // NB: "unsigned long int" is assumed to be 32 bits wide

int32_t random(void)  // RAND_MAX assumed to be 32767
{
    next = next * 1103515245 + 12345;
    return (unsigned int) (next / 65536) % 32768;
}

void seed_random(uint32_t seed)
{
    next = seed;
}

#endif