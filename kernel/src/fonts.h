#ifndef FONTS_H
#define FONTS_H

#define PSF1_FONT_MAGIC 0x0436

typedef struct {
    uint16_t magic; // Magic bytes for identification.
    uint8_t fontMode; // PSF font mode.
    uint8_t characterSize; // PSF character size.
} PSF1_Header;


#define PSF_FONT_MAGIC 0x864ab572

typedef struct {
    uint32_t magic;         /* magic bytes to identify PSF */
    uint32_t version;       /* zero */
    uint32_t headersize;    /* offset of bitmaps in file, 32 */
    uint32_t flags;         /* 0 if there's no unicode table */
    uint32_t numglyph;      /* number of glyphs */
    uint32_t bytesperglyph; /* size of each glyph */
    uint32_t height;        /* height in pixels */
    uint32_t width;         /* width in pixels */
} PSF_font;

                        
static uint32_t INTERNAL_ROW = 0;
static uint32_t INTERNAL_COLUMN = 0; 


extern PSF_font _binary_src_font_psf_start;

#define PIXEL_PADDING 8

PSF_font* get_pc_screen_font_handle() {
    return &_binary_src_font_psf_start;
}

//This function is to be used by kprint and not recommended directly since this can place a glyph anywhere on screen
void fill_glyph(const struct limine_framebuffer * frame_buffer, const uint32_t rgb) {
    PSF_font* font_handle = get_pc_screen_font_handle();
    if (!font_handle) {
        return; // Something bad must have occured (is the font embedded in the kernel?)
    }

    if (!frame_buffer) {
        return; // Something must have gone to shit if we can't paint on the screen.
    }

    const uint32_t max_columns =  (frame_buffer->width / font_handle->width) + 1;
    const uint32_t max_rows =  (frame_buffer->height / font_handle->height) + 1;

    uint32_t row = INTERNAL_ROW;
    uint32_t column = INTERNAL_COLUMN;

    volatile uint8_t* font_bmp = (uint8_t*)&_binary_src_font_psf_start;
    font_bmp += font_handle->headersize;

    uint32_t x = (column * font_handle->width) + PIXEL_PADDING; // Will be column
    uint32_t y = (row * font_handle->height) + PIXEL_PADDING; // Will be row

    for (uint32_t height = 0; height < font_handle->height; height++) {
        uint8_t row = font_bmp[height];
        for (uint32_t width = 0; width < font_handle->width; width++) {
                plot_pixel(frame_buffer, (x+(font_handle->height-width-1)+PIXEL_PADDING), (y+height+PIXEL_PADDING), rgb); 
        }
    }
}

//This function is to be used by kprint and not recommended directly since this can place a glyph anywhere on screen
void put_glyph(const struct limine_framebuffer * frame_buffer, uint8_t glyph_index, const uint32_t row, const uint32_t column, const uint32_t rgb) {
    PSF_font* font_handle = get_pc_screen_font_handle();
    if (!font_handle) {
        return; // Something bad must have occured (is the font embedded in the kernel?)
    }

    if (!frame_buffer) {
        return; // Something must have gone to shit if we can't paint on the screen.
    }

    volatile uint8_t* font_bmp = (uint8_t*)&_binary_src_font_psf_start;
    font_bmp += (font_handle->headersize + (glyph_index * font_handle->bytesperglyph));

    uint32_t x = (column * font_handle->width) + PIXEL_PADDING; // Will be column
    uint32_t y = (row * font_handle->height) + PIXEL_PADDING; // Will be row

    for (uint32_t height = 0; height < font_handle->height; height++) {
        uint8_t row = font_bmp[height];
        for (uint32_t width = 0; width < font_handle->width; width++) {
            if (((row & (1 << width)) >> width) == 1) {
                plot_pixel(frame_buffer, (x+(font_handle->height-width-1)+PIXEL_PADDING), (y+height+PIXEL_PADDING), rgb); 
            }
        }
    }
}

void set_cursor_position(uint32_t row, uint32_t column) {
    INTERNAL_ROW = row;
    INTERNAL_COLUMN = column;
}

#define reset_cursor_position() set_cursor_position(0,0)

//Internal kernel print function not to be confused with the standard C function printf
void printk(const struct limine_framebuffer * frame_buffer, char* string, const uint32_t rgb) {
    PSF_font* font_handle = get_pc_screen_font_handle();
    PSF_font* font_metadata = &_binary_src_font_psf_start;
    const uint32_t max_columns =  (frame_buffer->width / font_metadata->width) + 1;
    uint32_t row = INTERNAL_ROW;
    uint32_t column = INTERNAL_COLUMN;
    while(*string != '\0') {
        char c = *string;
        if (c == '\n' || column >= max_columns) {
            row++;
            column = 0;
            string++;
            continue;
        }
        put_glyph(frame_buffer, c, row, column, rgb);
        column++;
        string++;
    }

    INTERNAL_ROW = row;
    INTERNAL_COLUMN = column;

}



#endif