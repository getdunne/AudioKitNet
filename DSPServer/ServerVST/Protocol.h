#pragma once
#include <stdint.h>

#define MAXFRAMES 2048
#define CHANNELS 2
#define DATASIZE sizeof(float)
#define BUFLEN (MAXFRAMES * CHANNELS * DATASIZE)
#define ALL_PORTS_ADDR "0.0.0.0"    // listen on all available interfaces
#define LOOPBACK_ADDR "127.0.0.1"   // listen on loopback interface only
#define DEFAULT_ADDR ALL_PORTS_ADDR
#define DEFAULT_PORT 27016

#pragma pack(push, 1)

#define UINT16_BIT15_MASK       0x8000
#define UINT16_LOW15BITS_MASK   0x7FFF

struct ConnectHeader
{
    uint32_t    sampleRateHz;
    uint32_t    maxSamplesPerBlock;
};

struct SendDataHeader
{
    uint16_t    frameCount;
    uint16_t    midiCount;
    uint16_t    paramCount;
    uint16_t    padding;
    uint32_t    timeStamp;
};

struct MIDIMessageInfoStruct
{
    uint8_t     status;
    uint8_t     channel;
    uint8_t     data1;
    uint8_t     data2;
    uint32_t    startFrame;
};

struct ParamMessageStruct
{
    uint16_t    effectIndex;
    uint16_t    paramIndex;
    float       paramValue;
};

struct SampleDataHeader
{
    uint16_t    mainByteCount;      // or total bytecount if uncompressed
    uint16_t    corrByteCount;      // 0 = lossy compression, 0xFFFF = uncompressed
};

#pragma pack(pop)
