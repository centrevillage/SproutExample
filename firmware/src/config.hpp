#pragma once

#if defined(IGB_FIXED_SAMPLING_RATE)
# define SPR_SR IGB_FIXED_SAMPLING_RATE
#else
# define SPR_SR 96000
#endif

#define SPR_AUDIO_BUFFER_SIZE 48
// firmware vender id (replace your vendor id)
#define SPR_VENDOR_ID 0xFFFFFFFF
// firmware id (SMPL = sample firmware)
#define SPR_FIRMWARE_ID 0x534d504c
// firmware version ( major (msb) | minor (lsb))
#define SPR_FIRMWARE_VER (0UL << 16U | 1UL)
