#ifndef KERNEL_VECTOR_H
#define KERNEL_VECTOR_H

#include <stdint.h>
#include <memory.h>

typedef struct {
    char* data; // Underyling bytes of the vector we don't necessarily know how much each object occupies without a size in bytes
    size_t sizeof_object;
    size_t count;
    size_t capacity; // Capacity in objects not in bytes we
} kernel_array_t;

// Note Capacity in objects not bytes
void create_empty_kernel_array(kernel_array_t* array_handle, size_t sizeof_object, size_t capcacity) {
    
    if (NULL == array_handle) {
        return;
    }

    array_handle->capacity = capcacity;
    array_handle->count = 0;
    array_handle->sizeof_object = sizeof_object;
    array_handle->data = (char*)k_malloc(sizeof_object * capcacity);
    
}

bool index_kernel_array(kernel_array_t* array_handle, size_t index, void* out_object) {
    if (NULL == array_handle) {
        return false;
    }
    if (0 == array_handle->sizeof_object) {
        return false;
    }
    if (index >= array_handle->capacity) {
        return false;
    }
    memcpy(out_object, array_handle->data + (array_handle->sizeof_object * index), array_handle->sizeof_object);
}

bool override_at_kernel_array(kernel_array_t* array_handle, size_t index, void* object) {
     if (NULL == array_handle) {
        return false;
    }
    if (0 == array_handle->sizeof_object) {
        return false;
    }
    if (index >= array_handle->capacity) {
        return false;
    }

    memcpy(array_handle->data + (array_handle->sizeof_object * index), object, array_handle->sizeof_object);

}

bool append_to_kernel_array(kernel_array_t* array_handle, void* object) {
     if (NULL == array_handle) {
        return false;
    }
    if (0 == array_handle->sizeof_object) {
        return false;
    }
    if ((array_handle->count + 1) >= array_handle->capacity) {
        return false;
    }

    memcpy(array_handle->data + (array_handle->sizeof_object * array_handle->count), object, array_handle->sizeof_object);

    array_handle->count++;
}

#endif