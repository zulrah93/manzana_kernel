#ifndef KERNEL_STRING_H
#define KERNEL_STRING_H

#include <utility.h>
#include <memory.h>
#include <fonts.h>

//Used to create strings for kernel usage of fixed capacity
typedef struct {
    char* c_str;
    size_t capacity;
    size_t length;
} kernel_string;

void create_empty_kernel_string(kernel_string* new_string, size_t capacity) { // capacity will be one more for null teriminator
    if (NULL == new_string) {
        return;
    }
    new_string->capacity = capacity;
    new_string->c_str = (char*)k_malloc(capacity+1);
    memset(new_string->c_str, 0, capacity);
    new_string->length = 0;
}

void assign_c_str_to_kernel_string(kernel_string* new_string, char * c_str) {
    if (NULL == new_string) {
        return;
    }
    size_t length = string_length(c_str);
    if (length >= new_string->capacity) {
        return;
    }
    memcpy(new_string->c_str, c_str, length);
    new_string->length = length;

}

void append_c_str_to_kernel_string(kernel_string* new_string, char * c_str) {
    if (NULL == new_string) {
        return;
    }
    if (0 == new_string->length) {
        assign_c_str_to_kernel_string(new_string, c_str);
        return;
    }
    size_t length = string_length(c_str);
    size_t gross_length = length + new_string->length;
    if (gross_length >= new_string->capacity) {
        return;
    }
    memcpy(new_string->c_str + new_string->length, c_str, length);
    new_string->length += length;

}

void assign_integer_to_kernel_string(kernel_string* new_string, size_t integer) {
    if (NULL == new_string) {
        return;
    }
    char c_str[20] = {0};
    integer_to_string(c_str, integer);
    size_t length = string_length(c_str);
    if (length >= new_string->capacity) {
        return;
    }
    memcpy(new_string->c_str, c_str, length);
    new_string->length = length;

}

void assign_hex_to_kernel_string(kernel_string* new_string, size_t integer) {
    if (NULL == new_string) {
        return;
    }
    char c_str[20] = {0};
    integer_to_hex_string(c_str, integer);
    size_t length = string_length(c_str);
    if (length >= new_string->capacity) {
        return;
    }
    memcpy(new_string->c_str, c_str, length);
    new_string->length = length;

}

void assign_binary_to_kernel_string(kernel_string* new_string, size_t integer) {
    if (NULL == new_string) {
        return;
    }
    char c_str[65] = {0};
    integer_to_binary_string(c_str, integer);
    size_t length = string_length(c_str);
    if (length >= new_string->capacity) {
        return;
    }
    memcpy(new_string->c_str, c_str, length);
    new_string->length = length;

}

void append_integer_to_kernel_string(kernel_string* new_string, size_t integer) {
    if (NULL == new_string) {
        return;
    }
    if (0 == new_string->length) {
        assign_integer_to_kernel_string(new_string, integer);
        return;
    }
    char c_str[20] = {0};
    integer_to_string(c_str, integer);
    size_t length = string_length(c_str);
    size_t gross_length = length + new_string->length;
    if (gross_length >= new_string->capacity) {
        return;
    }
    memcpy(new_string->c_str + new_string->length, c_str, length);
    new_string->length += length;

}

void append_hex_to_kernel_string(kernel_string* new_string, size_t integer) {
    if (NULL == new_string) {
        return;
    }
    if (0 == new_string->length) {
        assign_hex_to_kernel_string(new_string, integer);
        return;
    }
    char c_str[20] = {0};
    integer_to_hex_string(c_str, integer);
    size_t length = string_length(c_str);
    size_t gross_length = length + new_string->length;
    if (gross_length >= new_string->capacity) {
        return;
    }
    memcpy(new_string->c_str + new_string->length, c_str, length);
    new_string->length += length;

}

void append_binary_to_kernel_string(kernel_string* new_string, size_t integer) {
    if (NULL == new_string) {
        return;
    }
    if (0 == new_string->length) {
        assign_binary_to_kernel_string(new_string, integer);
        return;
    }
    char c_str[65] = {0};
    integer_to_binary_string(c_str, integer);
    size_t length = string_length(c_str);
    size_t gross_length = length + new_string->length;
    if (gross_length >= new_string->capacity) {
        return;
    }
    memcpy(new_string->c_str + new_string->length, c_str, length);
    new_string->length += length;

}

void print_kernel_string(const struct limine_framebuffer * frame_buffer, const kernel_string string, const uint32_t rgb) {
    printk(frame_buffer, string.c_str, rgb);
}


#endif