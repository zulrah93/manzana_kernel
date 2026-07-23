#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limine.h>
#include <memory.h>
#include <vga.h>
#include <fonts.h>
#include <kernel_string.h>
#include <system.h>

// Set the base revision to 6, this is recommended as this is the latest
// base revision described by the Limine boot protocol specification.
// See specification for further info.

__attribute__((used, section(".limine_requests")))
static volatile uint64_t limine_base_revision[] = LIMINE_BASE_REVISION(6);

// The Limine requests can be placed anywhere, but it is important that
// the compiler does not optimise them away, so, usually, they should
// be made volatile or equivalent, _and_ they should be accessed at least
// once or marked as used with the "used" attribute as done here.

__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST_ID,
    .revision = 0
};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST_ID,
    .revision = 0
};

// Finally, define the start and end markers for the Limine requests.
// These can also be moved anywhere, to any .c file, as seen fit.

__attribute__((used, section(".limine_requests_start")))
static volatile uint64_t limine_requests_start_marker[] = LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
static volatile uint64_t limine_requests_end_marker[] = LIMINE_REQUESTS_END_MARKER;

// Halt and catch fire function.
static void halt_catch_fire(void) {
    for (;;) {
        asm ("wfi");
    }
}

// The following will be our kernel's entry point.
// If renaming kmain() to something else, make sure to change the
// linker script accordingly.
void kmain(void) {
    // Ensure the bootloader actually understands our base revision (see spec).
    if (LIMINE_BASE_REVISION_SUPPORTED(limine_base_revision) == false) {
        halt_catch_fire();
    }

    // Ensure we got a framebuffer.
    if (framebuffer_request.response == NULL
     || framebuffer_request.response->framebuffer_count < 1) {
        halt_catch_fire();
    }
    
    const size_t entry_count = memmap_request.response->entry_count; 
    struct limine_memmap_entry** entries = memmap_request.response->entries;

    init_memory_pool(entries, entry_count, true);

    bitmap_header_t* boot_logo_bmp_header = get_embedded_boot_logo();

    // Fetch the first framebuffer.
    struct limine_framebuffer *framebuffer = framebuffer_request.response->framebuffers[0];

    clear_screen(framebuffer, BLUE);
    draw_bitmap(framebuffer, boot_logo_bmp_header, 0, 280);
    reset_cursor_position();
    kernel_string kernel_buffer;
    create_empty_kernel_string(&kernel_buffer, 1024);
    append_c_str_to_kernel_string(&kernel_buffer, "Manzana Kernel\nCNTPCT_EL0 has a value of ");
    append_integer_to_kernel_string(&kernel_buffer, get_system_ticks());
    append_c_str_to_kernel_string(&kernel_buffer, " and we are at exception level ");
    append_integer_to_kernel_string(&kernel_buffer, get_current_exception_level().level);
    append_c_str_to_kernel_string(&kernel_buffer, " and SCTLR_EL1 is 0x");
    append_hex_to_kernel_string(&kernel_buffer, get_sctrl_el1());
    append_c_str_to_kernel_string(&kernel_buffer, "\nIs this machine little endian? ");
    append_c_str_to_kernel_string(&kernel_buffer, is_little_endian_system() ? "yes" : "no");
    print_kernel_string(framebuffer, kernel_buffer, WHITE);
    // We're done, just hang...
    halt_catch_fire();
}
