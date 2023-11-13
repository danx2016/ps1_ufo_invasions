#include <stdio.h>
#include <libcd.h>

#include "mem.h"
#include "cd.h"

void cd_init(void)
{
    while (!CdInit());

    // set cd speed at 2x 
    uint8_t param = CdlModeSpeed;
    CdControlB(CdlSetmode, &param, NULL);
}

void *cd_load_file(uint8_t *filename)
{
    CdlFILE file;
    uint8_t *data_buffer = NULL;
    if (CdSearchFile(&file, filename))
    {
        uint32_t num_sectors = (file.size + 2047) / 2048;
        uint32_t file_size = num_sectors * 2048;
        data_buffer = (uint8_t*) mem_alloc(file_size);
        CdControlB(CdlSetloc, (uint8_t*) &file.pos, 0);
        CdRead(num_sectors, (uint32_t*) data_buffer, CdlModeSpeed);
        int32_t ret = CdReadSync(0, NULL);

        // error
        if (ret < 0)
        {
            mem_free(data_buffer);
            return NULL;
        }
    }
    return data_buffer;
}

static CdlFILE xa_file;
static int32_t xa_music_end_pos;

static void cd_xa_auto_replay(uint8_t status, uint8_t *result)
{
    if (status == CdlDataReady)
    {
        uint8_t data_buffer[2340];
        CdGetSector(data_buffer, 585);
        CdlLOC *pos = (CdlLOC*) data_buffer;
        if (CdPosToInt(pos) >= xa_music_end_pos)
        {
            CdControlF(CdlReadS, (uint8_t*) &xa_file.pos);
        }
    }
}

void cd_play_music_xa(uint8_t *filename)
{
    if (!CdSearchFile(&xa_file, filename))
    {
        printf("xa file %s not found ! \n", filename);
        return;
    }

    xa_music_end_pos = CdPosToInt(&xa_file.pos) + (xa_file.size / 2048) - 1; 

    uint8_t param = CdlModeSpeed | CdlModeRT | CdlModeSF | CdlModeSize1;
    CdControlB(CdlSetmode, &param, NULL);
    
    CdlFILTER filter;
    filter.file = 1;
    filter.chan = 0;
    CdControlB(CdlSetfilter, (uint8_t*) &filter, NULL);

    CdReadyCallback(cd_xa_auto_replay);
    CdControlF(CdlReadS, (uint8_t*) &xa_file.pos);
}

void cd_stop_music_xa()
{
    CdControlB(CdlPause, NULL, NULL);
}