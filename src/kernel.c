#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#if defined(__linux__) || !defined(__i386__)
#error "This file must be compiled with an i?86-elf cross-compiler"
#endif

/******** VGA Text Mode Driver ********/

#define VGA_TEXT_MEM_ADDR 0xB8000

#define VGA_COLOR_BITS 4
#define VGA_COLOR_MASK ((1 << VGA_COLOR_BITS) - 1)

typedef unsigned char vga_text_char_t;
#define VGA_TEXT_CHAR_BITS (sizeof(vga_text_char_t) * 8)

typedef uint8_t vga_text_attr_t;
typedef uint16_t vga_text_entry_t;

typedef enum vga_color {
    VGA_COLOR_BLACK         =  0,
    VGA_COLOR_BLUE          =  1,
    VGA_COLOR_GREEN         =  2,
    VGA_COLOR_CYAN          =  3,
    VGA_COLOR_RED           =  4,
    VGA_COLOR_MAGENTA       =  5,
    VGA_COLOR_BROWN         =  6,
    VGA_COLOR_LIGHT_GREY    =  7,
    VGA_COLOR_DARK_GREY     =  8,
    VGA_COLOR_LIGHT_BLUE    =  9,
    VGA_COLOR_LIGHT_GREEN   = 10,
    VGA_COLOR_LIGHT_CYAN    = 11,
    VGA_COLOR_LIGHT_RED     = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN   = 14,
    VGA_COLOR_WHITE         = 15,
} vga_color_t;

static inline vga_text_attr_t vga_text_attr(vga_color_t fg, vga_color_t bg)
{
    return (fg & VGA_COLOR_MASK) | (bg & VGA_COLOR_MASK) << VGA_COLOR_BITS;
}

static inline vga_text_entry_t vga_text_entry(vga_text_char_t c, vga_text_attr_t attr)
{
    return (vga_text_entry_t) c | (vga_text_entry_t) attr << VGA_TEXT_CHAR_BITS;
}

/******** String Routines ********/

size_t strlen(const char* str)
{
    size_t len = 0;
    while (str[len]) {
        len++;
    }
    return len;
}

/******** Terminal Driver ********/

#define TERMINAL_WIDTH ((size_t) 80)
#define TERMINAL_HEIGHT ((size_t) 25)

size_t terminal_row;
size_t terminal_column;
vga_text_attr_t terminal_attr;
vga_text_entry_t *terminal_buffer;

void terminal_initialize()
{
    terminal_row = 0;
    terminal_column = 0;
    terminal_attr = vga_text_attr(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    terminal_buffer = (vga_text_entry_t*) VGA_TEXT_MEM_ADDR;
    for (size_t y = 0; y < TERMINAL_HEIGHT; y++) {
        for (size_t x = 0; x < TERMINAL_WIDTH; x++) {
            terminal_buffer[y * TERMINAL_WIDTH + x] = vga_text_entry(' ', terminal_attr);
        }
    }
}

void terminal_set_attr(vga_text_attr_t attr)
{
    terminal_attr = attr;
}

void terminal_put_entry_at(char c, vga_text_attr_t attr, size_t x, size_t y)
{
    terminal_buffer[y * TERMINAL_WIDTH + x] = vga_text_entry(c, attr);
}

void terminal_put_char(char c)
{
    terminal_put_entry_at(c, terminal_attr, terminal_column, terminal_row);
    terminal_column++;
    if (terminal_column == TERMINAL_WIDTH) {
        terminal_column = 0;
        terminal_row++;
        if (terminal_row == TERMINAL_HEIGHT) {
            terminal_row = 0;
        }
    }
}
