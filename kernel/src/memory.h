#ifndef K_MALLOC_H
#define K_MALLOC_H

#include <limine.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <utility.h>

#define SLIDE_ADDRESS (0xffffffff80000000)

// GCC and Clang reserve the right to generate calls to the following
// 4 functions even if they are not directly called.
// Implement them as the C specification mandates.
// DO NOT remove or rename these functions, or stuff will eventually break!
// They CAN be moved to a different .c file.

void *memcpy(void *restrict dest, const void *restrict src, size_t n) {
    uint8_t *restrict pdest = (uint8_t *restrict)dest;
    const uint8_t *restrict psrc = (const uint8_t *restrict)src;

    for (size_t i = 0; i < n; i++) {
        pdest[i] = psrc[i];
    }

    return dest;
}

void *memset(void *s, int c, size_t n) {
    uint8_t *p = (uint8_t *)s;

    for (size_t i = 0; i < n; i++) {
        p[i] = (uint8_t)c;
    }

    return s;
}

void *memmove(void *dest, const void *src, size_t n) {
    uint8_t *pdest = (uint8_t *)dest;
    const uint8_t *psrc = (const uint8_t *)src;

    if (src > dest) {
        for (size_t i = 0; i < n; i++) {
            pdest[i] = psrc[i];
        }
    } else if (src < dest) {
        for (size_t i = n; i > 0; i--) {
            pdest[i-1] = psrc[i-1];
        }
    }

    return dest;
}

int memcmp(const void *s1, const void *s2, size_t n) {
    const uint8_t *p1 = (const uint8_t *)s1;
    const uint8_t *p2 = (const uint8_t *)s2;

    for (size_t i = 0; i < n; i++) {
        if (p1[i] != p2[i]) {
            return p1[i] < p2[i] ? -1 : 1;
        }
    }

    return 0;
}

// Meta data about our memory allocations

typedef struct {
    size_t block_size_bytes;
    bool is_free;

} heap_header_t;

// Global variables to manage heap and constants
#define HEAP_CAPACITY 50000
static heap_header_t* start_of_heap = NULL;
static size_t heap_usage_bytes = 0; // Includes meta information as well 
static uint8_t  stack[HEAP_CAPACITY];

bool init_memory_pool(struct limine_memmap_entry** entries, const size_t entries_size, bool use_stack_allocation) {
    
    if (use_stack_allocation) {
         memset(stack, 0, HEAP_CAPACITY);
         start_of_heap = (heap_header_t*)&stack[0];
         start_of_heap->is_free = true;
         start_of_heap->block_size_bytes = 0;
         return true;
    }
    
    if (NULL == entries) {
        return false;
    }

    for(size_t index = 0; index < entries_size; index++) {
        if (entries[index]->type == LIMINE_MEMMAP_USABLE) {
            start_of_heap = (heap_header_t*)(entries[index]->base);
            memset(start_of_heap, 0, sizeof(heap_header_t));
            start_of_heap->is_free = true;
            start_of_heap->block_size_bytes = 0;
            return true;
        }
    }

    heap_usage_bytes = 0;
    
    return NULL != start_of_heap;
}


void* k_malloc(size_t bytes) {
    if (NULL == start_of_heap) {
        return NULL;
    }

    if ((heap_usage_bytes + (sizeof(heap_header_t) + bytes)) >= HEAP_CAPACITY) {
        return NULL;
    }

    if (0 == start_of_heap->block_size_bytes) {
        start_of_heap->block_size_bytes = bytes;
        start_of_heap->is_free = false;
        heap_header_t* next_free_header = (heap_header_t*)((char*)start_of_heap + sizeof(heap_header_t) + start_of_heap->block_size_bytes);
        next_free_header->block_size_bytes = 0;
        next_free_header->is_free = true;
        heap_usage_bytes += (sizeof(heap_header_t) + bytes);
        return (start_of_heap + 1);
    }

    heap_header_t* head = (heap_header_t*)((char*)start_of_heap + sizeof(heap_header_t) +  start_of_heap->block_size_bytes);
    
    while (!head->is_free) {
        head = (heap_header_t*)((char*)head + sizeof(heap_header_t) + head->block_size_bytes);
        if (head->is_free && ((head->block_size_bytes == 0) || head->block_size_bytes >= bytes)) {
            break;
        }
    }
    
    if (0 == head->block_size_bytes) {
        head->block_size_bytes = bytes;
    }
    
    head->is_free = false;
    heap_header_t* next_free_header = (heap_header_t*)((char*)head + sizeof(heap_header_t) + head->block_size_bytes);
    next_free_header->block_size_bytes = 0;
    next_free_header->is_free = true;
    heap_usage_bytes += (sizeof(heap_header_t) + bytes);
    return (head + 1);
}


void k_free(void* block) {
    if (NULL == block) {
        return;
    }
    if (0 == heap_usage_bytes) {
        return;
    }
    heap_header_t* block_header = (heap_header_t*)block - 1;
    block_header->is_free = true;
    heap_usage_bytes -= (sizeof(heap_header_t) + block_header->block_size_bytes);
}

//Note: Use k_free since this is just convience for k_malloc string allocation
char* cstring_alloc(char* c_string) {
    if (NULL == c_string) {
        return NULL;
    }
    size_t bytes = string_length(c_string);
    char* heap_string = (char*)k_malloc(bytes);
    memcpy(heap_string, c_string, bytes);
    return heap_string;
}

#endif