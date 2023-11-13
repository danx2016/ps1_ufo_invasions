#include "gfx.h"
#include "gfx_font.h"
#include "asset.h"

static FONT *fonts[FONTS_MAX_NUM];
static FONT *current_font = NULL;

void gfx_font_add(size_t font_id, FONT *font, size_t tileset_id)
{
    fonts[font_id] = font;
    font->tileset_id = tileset_id;
    gfx_add_tileset(tileset_id, font->tim_image, 1, 1);
}

void gfx_font_use(size_t font_id)
{
    current_font = fonts[font_id];
    gfx_use_tileset(current_font->tileset_id);
}

void gfx_font_draw_text(uint8_t *text, int16_t x, int16_t y)
{
    uint32_t char_index = 0;
    uint8_t current_char;
    while (current_char = text[char_index++])
    {
        FONT_CHAR *ch = &current_font->chars[current_char];

        // draw the char
        gfx_draw_custom_tile(ch->x, ch->y, ch->width, ch->height, x + ch->x_offset, y + ch->y_offset);

        // update the cursor position
        x += ch->x_advance;
    }    
}
