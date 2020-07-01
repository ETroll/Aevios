#include "graphics.h"

ui_window* efi_ui_window_new(EFI_SYSTEM_TABLE* ST, ui_context* ctx, unsigned int x, unsigned int y, unsigned int width, unsigned int height) 
{
    ui_window* window;
    EFI_STATUS status; 

    status = ST->BootServices->AllocatePool(EfiLoaderData, sizeof(ui_window), (void**)&window);

    if(EFI_ERROR(status))
    {
        return NULL;
    }

    window->x = x;
    window->y = y;
    window->width = width;
    window->height = height;
    window->ctx = ctx;

    efi_util_list_add(ST, window->ctx->items, window);

    return window;
}

void efi_ui_window_delete(EFI_SYSTEM_TABLE* ST, ui_window* window)
{
    ST->BootServices->FreePool(window);
}

uint8_t pseudo_rand_8() {

    static uint16_t seed = 0;
    return (uint8_t)(seed = (12657 * seed + 12345) % 256);
}

void efi_ui_window_paint(ui_window* window)
{
    //Assume BGRR - But check for BRGR later on the GOP
    EFI_GRAPHICS_OUTPUT_BLT_PIXEL color = {pseudo_rand_8(), pseudo_rand_8(), pseudo_rand_8(), 0};

    efi_ui_context_fillRect(window->ctx, window->x, window->y, window->width, window->height, &color);
}