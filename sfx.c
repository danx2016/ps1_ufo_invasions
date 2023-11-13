#include <libspu.h>

#include "sfx.h"

static uint32_t spu_addr;

void sfx_init(void)
{
    SpuInit();
    
    SpuCommonAttr sca;
    sca.mask = (
        SPU_COMMON_MVOLL | 
        SPU_COMMON_MVOLR | 
        SPU_COMMON_CDVOLL | 
        SPU_COMMON_CDVOLR | 
        SPU_COMMON_CDMIX
    );
    
    sca.mvol.left = 0x1fff;
    sca.mvol.right = 0x1fff;
    sca.cd.volume.left = 0x1fff;
    sca.cd.volume.right = 0x1fff;
    sca.cd.mix = SPU_ON;
    SpuSetCommonAttr(&sca);

    spu_addr = SFX_SPU_START_ADDR;
}

void sfx_add_sound(size_t sound_id, uint8_t *sound_data)
{
    uint32_t s0 = ((uint32_t) sound_data[12]) << 24;
    uint32_t s1 = ((uint32_t) sound_data[13]) << 16;
    uint32_t s2 = ((uint32_t) sound_data[14]) << 8;
    uint32_t s3 = ((uint32_t) sound_data[15]);
    uint32_t sound_size = s0 + s1 + s2 + s3;

    SpuSetTransferMode(SPU_TRANSFER_BY_DMA);
    SpuSetTransferStartAddr(spu_addr);
    SpuWrite(sound_data + SFX_VAG_HEADER_SIZE, sound_size);
    SpuIsTransferCompleted(SPU_TRANSFER_WAIT);

    int32_t voice_num = 1 << sound_id;

    SpuVoiceAttr voice_attr;
    voice_attr.mask = (
        SPU_VOICE_VOLL |
        SPU_VOICE_VOLR |
        SPU_VOICE_PITCH |
        SPU_VOICE_WDSA |
        SPU_VOICE_ADSR_AMODE |
        SPU_VOICE_ADSR_SMODE |
        SPU_VOICE_ADSR_RMODE |
        SPU_VOICE_ADSR_AR |
        SPU_VOICE_ADSR_DR |
        SPU_VOICE_ADSR_SR |
        SPU_VOICE_ADSR_RR |
        SPU_VOICE_ADSR_SL
    );

    voice_attr.volume.left = 0x3fff;
    voice_attr.volume.right = 0x3fff;
    voice_attr.pitch = 4096;
    voice_attr.a_mode = SPU_VOICE_LINEARIncN;
    voice_attr.s_mode = SPU_VOICE_LINEARIncN;
    voice_attr.r_mode = SPU_VOICE_LINEARDecN;
    voice_attr.ar = 0x0;
    voice_attr.dr = 0x0;
    voice_attr.sr = 0x0;
    voice_attr.rr = 0x0;
    voice_attr.sl = 0xf;

    voice_attr.voice = 1 << sound_id;
    voice_attr.addr = spu_addr;
    SpuSetVoiceAttr (&voice_attr);
    SpuSetKey(SpuOff, voice_num);

    spu_addr += sound_size;
}

void sfx_play_sound(size_t sound_id)
{
    int32_t voice_num = 1 << sound_id;
    SpuSetKey(SPU_ON, voice_num);
}
