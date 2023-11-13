
#include <stdio.h>
#include <libcd.h>
#include <libpress.h>

#include "gfx.h"
#include "fmv.h"

bool fmv_is_play_finished;

#define RING_SIZE 32
static uint32_t ring_buffer[RING_SIZE * SECTOR_SIZE];

static uint32_t fmv_frame_end;

void fmv_init(uint8_t *filename, uint32_t frame_end)
{
    DecDCTReset(0);
    StSetRing(ring_buffer, RING_SIZE);
    StSetStream(0, 1, 0xffffffff, 0, 0);
    
    CdlFILE file;
    if (!CdSearchFile(&file, filename))
    {
        printf("str file %s not found !\n", filename);
        return;
    }

    CdControlB(CdlSetloc, (uint8_t*) &file.pos, NULL);
    CdRead2(CdlModeSpeed | CdlModeRT | CdlModeStream);
    fmv_is_play_finished = false;
    fmv_frame_end = frame_end;
}

static uint8_t buffer[0xefff];

void fmv_render_next_frame() {
    if (fmv_is_play_finished)
    {
        return;
    }

    uint32_t *addr;
    StHEADER *sector;
    while (StGetNext((uint32_t**) &addr, (uint32_t**) &sector))
    {
        if (!fmv_is_play_finished && sector->frameCount >= fmv_frame_end)
        {
            fmv_is_play_finished = true;
            DecDCToutCallback(0);
            StUnSetRing();
            CdControlB(CdlPause, 0, 0);
            return;
        }
    }
    
    //int32_t buffer_size = DecDCTBufSize(addr);
    //uint8_t buffer[4 * buffer_size + 4];

    DecDCTvlc(addr, (uint32_t*) buffer);
    DecDCTin((uint32_t*) buffer, 0); // 0->16 bits image
    uint8_t buffer_out[512]; // 16x16 macro block needs 512 bytes for 16 bits image
    for (int col = 0; col < 16; col++)
    {
        for (int row = 0; row < 15; row++)
        {
            DecDCTout((uint32_t*) buffer_out, 128); 
            DecDCToutSync(0);

            RECT r2;
            int dy = gfx_db ? GFX_SCREEN_HEIGHT : 0;
            setRECT(&r2, col * 16, row * 16 + dy, 16, 16);
            LoadImage(&r2, (uint32_t*) buffer_out);
        }
    }
    StFreeRing(addr);
}