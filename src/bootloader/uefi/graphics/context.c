#include "graphics.h"

ui_context* efi_ui_context_new(EFI_SYSTEM_TABLE* ST) 
{
    ui_context* ctx;
    EFI_STATUS status;

    status = ST->BootServices->AllocatePool(EfiLoaderData, sizeof(ui_context), (void**)&ctx);
    if(EFI_ERROR(status))
    {
        return NULL;
    }

    EFI_GRAPHICS_OUTPUT_PROTOCOL *gop;
    EFI_GUID graphicsProtocolGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;

    status = ST->BootServices->LocateProtocol(&graphicsProtocolGuid, NULL, (void **)&gop);
    if(!EFI_ERROR(status)) 
    {
        if (gop->Mode)
        {
            for(int i = 0; i < gop->Mode->MaxMode; i++) 
            {
                EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info;
                UINTN size;

                status = gop->QueryMode(gop, i, &size, &info);
                if (EFI_ERROR(status) && status == EFI_NOT_STARTED) 
                {
                    efi_util_printf(ST->ConOut, L"EFI_GRAPHICS_OUTPUT_PROTOCOL->QueryMode() returned %r\n", status);
                    efi_util_printf(ST->ConOut, L"Trying to start EFI_GRAPHICS_OUTPUT_PROTOCOL with SetMode().\n");
                    status = gop->SetMode(gop, gop->Mode ? gop->Mode->Mode : 0);
                    status = gop->QueryMode(gop, i, &size, &info);
                }

                if (EFI_ERROR(status))
                {
                    efi_util_printf(ST->ConOut, L"%d: Bad response from QueryMode: %r (%d)\n", i, status, status);
                    continue;
                }

                if (CompareMem(info, gop->Mode->Info, sizeof(*info)))
                {
                    continue;
                }

                efi_util_printf(ST->ConOut, L"Found graphics mode %d with resolution %dx%d\r\n", i, info->HorizontalResolution, info->VerticalResolution);
                ctx->width = info->HorizontalResolution;
                ctx->height = info->VerticalResolution;
                ctx->gop = gop;

                UINT32 bufferSize = info->VerticalResolution * info->HorizontalResolution * sizeof(UINT32);

                status = ST->BootServices->AllocatePool(EfiLoaderData, bufferSize, (void**)&ctx->framebuffer);
                if(!EFI_ERROR(status)) 
                {
                    return ctx;
                }
                efi_util_printf(ST->ConOut, L"Allocation of 0x%08lx bytes failed.\n", bufferSize);
            }
        }
        efi_util_printf(ST->ConOut, L"EFI_GRAPHICS_OUTPUT_PROTOCOL->Mode is NULL\n");
    }
    else
    {
        CHAR16 tmp[100];
        efi_util_statusToString(tmp, status);
        efi_util_printf(ST->ConOut, L"Error locating Graphics output. Status: %lX - %s\r\n", status, tmp);
    }

    //We failed. Let us free what resources we have allocated
    efi_ui_context_delete(ST, ctx);
    return NULL;
}

void efi_ui_context_delete(EFI_SYSTEM_TABLE* ST, ui_context* ctx)
{
    if(ctx->framebuffer)
    {
        ST->BootServices->FreePool(ctx->framebuffer);
    }
    ST->BootServices->FreePool(ctx);
}

void efi_ui_context_paint(ui_context* ctx)
{
    ctx->gop->Blt(
        ctx->gop,
        (EFI_GRAPHICS_OUTPUT_BLT_PIXEL *)ctx->framebuffer,
        EfiBltBufferToVideo,
        0, 0, 0, 0,
        ctx->width,
        ctx->height,
        0);
}

void efi_ui_context_fillRect(ui_context* context, unsigned int x, unsigned int y, unsigned int width, unsigned int height, EFI_GRAPHICS_OUTPUT_BLT_PIXEL* color) 
{
    unsigned int cur_x;
    unsigned int max_x = x + width;
    unsigned int max_y = y + height;

    //Make sure we don't try to draw outside of the framebuffer:
    if(max_x > context->width)
    {
        max_x = context->width;
    }

    if(max_y > context->height)
    {
        max_y = context->height;
    }

    //Draw the rectangle into the framebuffer line-by line
    //(bonus points if you write an assembly routine to do it faster)
    for(; y < max_y; y++) 
    {
        for(cur_x = x; cur_x < max_x; cur_x++) 
        {
            context->framebuffer[y * context->width + cur_x] = *(UINT32*)color;
        }
    }
}