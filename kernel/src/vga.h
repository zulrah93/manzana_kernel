#ifndef VGA_H
#define VGA_H

#include <utility.h>

#pragma pack(push, 1)
typedef struct
{
    char magic_field[2];
    uint32_t bitmap_total_size;
    uint32_t reserved;
    uint32_t offset_to_pixels;
    uint32_t header_size;
    int32_t width;
    int32_t height;
    uint16_t plane_count;
    uint16_t bits_per_pixel;
    uint32_t compression_method;
    uint32_t data_size;
    uint32_t horizontal_resolution;
    uint32_t vertical_resolution;
    uint32_t color_pallete_count;
    uint32_t important_colors_used;
} bitmap_header_t;
#pragma pack(pop)

extern bitmap_header_t _binary_src_boot_logo_bmp_start;

bitmap_header_t *get_embedded_boot_logo()
{
    return (bitmap_header_t *)&_binary_src_boot_logo_bmp_start;
}

// Takes three bytes (red, green, blue) and returns a 32-bit integer representing the color
int32_t from_rgb(int8_t red, int8_t green, int8_t blue)
{
    return (red << 16) | (green << 8) | blue;
}

#define RED 0xff0000
#define GREEN 0x00ff00
#define BLUE 0x0000ff
#define WHITE 0xffffff
#define BLACK 0x000000

void clear_screen(const struct limine_framebuffer *frame_buffer, const int32_t rgb)
{
    if (!frame_buffer)
        return;

    uint32_t width = frame_buffer->width;
    uint32_t height = frame_buffer->height;

    int32_t *vga_buffer = (int32_t *)frame_buffer->address;

    if (!vga_buffer)
        return;

    for (uint32_t x = 0; x < (width * height); x++)
    {
        vga_buffer[x] = rgb;
    }
}

void plot_pixel(const struct limine_framebuffer *frame_buffer, const uint32_t x, const uint32_t y, const int32_t rgb)
{

    if (!frame_buffer)
        return;

    uint32_t width = frame_buffer->width;
    uint32_t height = frame_buffer->height;

    if (x > width) // Out of bounds
        return;
    if (y > height) // Out of bounds
        return;

    int32_t *vga_buffer = (int32_t *)frame_buffer->address;

    if (!vga_buffer)
        return;

    vga_buffer[(y * width) + x + 1] = rgb;
}

void draw_bitmap(const struct limine_framebuffer *frame_buffer, const bitmap_header_t *header, const uint32_t x, const uint32_t y)
{

    if (NULL == frame_buffer)
    {
        return;
    }
    if (NULL == header)
    {
        return;
    }
    if (!(header->magic_field[0] == 'B' && header->magic_field[1] == 'M'))
    {
        return;
    }

    if (header->bits_per_pixel != 32) {
        return; // We only support 32bpp and uncompressed (raw)
    }

    int32_t *pixel = (int32_t *)(header+1); // Getting a pointer to the actual pixel data screw the header

    if (NULL == pixel) {
        return;
    }

    uint32_t y_offset = 0;
    const uint32_t pixel_count = (header->width * header->height);
    const uint32_t pixel_padding = 8;

    for (int32_t pixel_index = 0; pixel_index < pixel_count; pixel_index++)
    {
        if ((pixel_index != 0) && (0 == (pixel_index % header->width))) {
            y_offset++;
        }
        size_t true_index = pixel_count - pixel_index - 1;
        int32_t current_pixel = pixel[true_index];
        plot_pixel(frame_buffer, 
            x + (true_index % header->width) + pixel_padding, 
            y + y_offset + pixel_padding, current_pixel);
    }

    return;
}

#endif