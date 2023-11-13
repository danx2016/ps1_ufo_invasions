#include <memory.h>
#include <libgpu.h>

#include "asset.h"
#include "mem.h"
#include "cd.h"
#include "gfx_font.h"

uint8_t *asset_load(uint8_t *filename)
{
    uint8_t *data = NULL;

    // try until success
    while ((data = cd_load_file(filename)) == NULL);

    return data;
}

static void transfer_image_to_vram(TIM_IMAGE *tim_image)
{
    LoadImage(tim_image->prect, tim_image->paddr);
    if (tim_image->mode & 0x8)
    {
        LoadImage(tim_image->crect, tim_image->caddr);
    }
    DrawSync(0);
}

TIM_IMAGE *asset_load_image(uint8_t *filename)
{
    uint8_t *image_data = asset_load(filename);
    TIM_IMAGE *tim_image = (TIM_IMAGE*) mem_alloc(sizeof(TIM_IMAGE));
    OpenTIM((uint32_t*) image_data);
    ReadTIM(tim_image);
    
    // copy the rect information so image_data can be freed later
    RECT *crect = (RECT*) mem_alloc(sizeof(RECT));
    RECT *prect = (RECT*) mem_alloc(sizeof(RECT));
    memcpy(crect, tim_image->crect, sizeof(RECT));
    memcpy(prect, tim_image->prect, sizeof(RECT));
    tim_image->crect = crect;
    tim_image->prect = prect;

    transfer_image_to_vram(tim_image);

    // once image was transferred to vram, it can be freed
    mem_free(image_data);

    return tim_image;
}

FONT *asset_load_font(uint8_t *bin_filename, uint8_t *tim_filename)
{
    FONT *font = (FONT*) asset_load(bin_filename);
    TIM_IMAGE *tim_image = asset_load_image(tim_filename);;
    font->tim_image = tim_image;
    return font;
}
