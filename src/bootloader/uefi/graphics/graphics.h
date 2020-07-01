#ifndef INCLUDE_BOOTGRAPHICS_H
#define INCLUDE_BOOTGRAPHICS_H

#include "../util/util.h"
#include "../efi/efi.h"
#include "../efi/protocol/efi-stop.h"
#include "../efi/protocol/efi-gop.h"

typedef struct ui_ontext_struct
{
    EFI_GRAPHICS_OUTPUT_PROTOCOL* gop;
    uint32_t* framebuffer;
    unsigned int width;
    unsigned int height; 
} ui_context;

typedef struct ui_window_struct
{
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
    ui_context* ctx;
} ui_window;


ui_window* efi_ui_window_new(EFI_SYSTEM_TABLE* st, ui_context* ctx, unsigned int x, unsigned int y, unsigned int width, unsigned int height);
void efi_ui_window_delete(EFI_SYSTEM_TABLE* ST, ui_window* window);
void efi_ui_window_paint(ui_window* window);

ui_context* efi_ui_context_new(EFI_SYSTEM_TABLE* ST);
void efi_ui_context_delete(EFI_SYSTEM_TABLE* ST, ui_context* ctx);
void efi_ui_context_paint(ui_context* ctx);
void efi_ui_context_fillRect(ui_context* context, unsigned int x, unsigned int y, unsigned int width, unsigned int height, EFI_GRAPHICS_OUTPUT_BLT_PIXEL* color);

#endif