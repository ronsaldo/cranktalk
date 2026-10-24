#include "crank-talk/sound.h"
#include "crank-talk/bytes.h"
#include "crank-talk/context.h"
#include "crank-talk/errors.h"
#include "crank-talk/message.h"
#include "crank-talk/string.h"
#include <stdlib.h>
#include <stdio.h>

#define WAVE_FORMAT_PCM 1

typedef struct crank_talk_WaveFileHeader_s
{
    char chunkID[4]; // RIFF
    uint32_t chunkSize;
    char waveID[4]; // WAVE
} crank_talk_WaveFileHeader_t;

typedef struct crank_talk_WaveFormatChunk_s
{
    char chunkID[4]; // "fmt "
    uint32_t chunkSize;
    struct {
        uint16_t formatTag;
        uint16_t numberOfChannels;
        uint32_t samplesPerSecond;
        uint32_t averageBytesPerSecond;
        uint16_t blockAlign;
        uint16_t bitsPerSample;
        uint16_t cbSize;
        uint16_t validBitsPerSample;
        uint32_t channelMask;
        uint8_t subFormat[16];
    } fmtChunk;
} crank_talk_WaveFormatChunk_t;

typedef struct crank_talk_DataChunkHeader_s
{
    char chunkID[4]; // "data"
    uint32_t chunkSize;
} crank_talk_DataChunkHeader_t;

static void
crank_talk_SoundSample_fromFileNamed_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_oop_t fileNameValue = primContext->arguments[1];

    char *fileName = crank_talk_String_asCString(fileNameValue);
    FILE *file = fopen(fileName, "rb");
    free(fileName);

    if(!file)
        crank_talk_errorPrintf(primContext->context, "Failed to open wave file.");

    // Read the header
    crank_talk_WaveFileHeader_t fileHeader = {};
    if(fread(&fileHeader, sizeof(crank_talk_WaveFileHeader_t), 1, file) != 1)
    {
        fclose(file);
        crank_talk_errorPrintf(primContext->context, "Failed to read wave file header.");
    }

    if(memcmp(fileHeader.chunkID, "RIFF", 4) || memcmp(fileHeader.waveID, "WAVE", 4))
    {
        fclose(file);
        crank_talk_errorPrintf(primContext->context, "Invalid wave header.");
    }

    // Read the format chunk.
    crank_talk_WaveFormatChunk_t formatChunk = {};
    if(fread(&formatChunk, 8, 1, file) != 1)
    {
        fclose(file);
        crank_talk_errorPrintf(primContext->context, "Failed to read wave fmt chunk.");
    }

    if(memcmp(formatChunk.chunkID, "fmt ", 4))
    {
        fclose(file);
        crank_talk_errorPrintf(primContext->context, "Invalid wave fmt chunk header.");
    }

    if(fread(&formatChunk.fmtChunk, formatChunk.chunkSize, 1, file) != 1)
    {
        fclose(file);
        crank_talk_errorPrintf(primContext->context, "Failed to read wave fmt chunk.");
    }

    if(formatChunk.fmtChunk.formatTag != WAVE_FORMAT_PCM)
    {
        fclose(file);
        crank_talk_errorPrintf(primContext->context, "Unsupported wave format tag.");
    }

    // Data
    crank_talk_DataChunkHeader_t dataChunkHeader = {};
    if(fread(&dataChunkHeader, sizeof(dataChunkHeader), 1, file) != 1)
    {
        fclose(file);
        crank_talk_errorPrintf(primContext->context, "Failed to read wave data chunk header.");
    }

    crank_talk_ByteArray_t *data = (crank_talk_ByteArray_t*)crank_talk_ByteArray_create(primContext->context, dataChunkHeader.chunkSize);
    if(fread(data->elements, dataChunkHeader.chunkSize, 1, file) != 1)
    {
        fclose(file);
        crank_talk_errorPrintf(primContext->context, "Failed to read wave data chunk content.");
    }

    fclose(file);

    size_t sampleCount = dataChunkHeader.chunkSize / formatChunk.fmtChunk.numberOfChannels / (formatChunk.fmtChunk.bitsPerSample / 8);
    crank_talk_ByteArray_t *leftSamples = NULL;
    crank_talk_ByteArray_t *rightSamples = NULL;
    if(formatChunk.fmtChunk.numberOfChannels == 1)
    {
        crank_talk_ByteArray_t *floatSamples = (crank_talk_ByteArray_t *)crank_talk_ByteArray_create(primContext->context, sampleCount*sizeof(float));
        int16_t *sourceSamples = (int16_t*)data->elements;
        float *destSamples = (float*)floatSamples->elements;
        for(size_t i = 0; i < sampleCount; ++i)
            destSamples[i] = (float)sourceSamples[i] / (float)INT16_MAX;

        leftSamples = floatSamples;
        rightSamples = floatSamples;
    }
    else
    {
        crank_talk_errorPrintf(primContext->context, "TODO: support wave files with %d channels.\n", formatChunk.fmtChunk.numberOfChannels);
    }

    crank_talk_SoundSample_t *sample = (crank_talk_SoundSample_t *)CrankAllocateValuesObjectWithClassName(primContext->context, SoundSample);
    sample->duration = crank_talk_oop_fromFloat(primContext->context, (float)sampleCount / (float)formatChunk.fmtChunk.samplesPerSecond);
    sample->sampleCount = crank_talk_oop_encodeSmallInteger(sampleCount);
    sample->sampleRate = crank_talk_oop_encodeSmallInteger(formatChunk.fmtChunk.samplesPerSecond);
    sample->leftSamples = leftSamples;
    sample->rightSamples = rightSamples;
    return crank_talk_Primitive_return(primContext, (crank_talk_oop_t)sample);
}

static void
crank_talk_SoundSource_play_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 1) return crank_talk_Primitive_fail(primContext);
    crank_talk_oop_t self = primContext->arguments[0];

    crank_talk_SoundSource_t *source = (crank_talk_SoundSource_t*)self;

    crank_talk_Mutex_lock(&primContext->context->soundSystemMutex);
    crank_talk_SoundSystem_t *soundSystem = (crank_talk_SoundSystem_t *)primContext->context->roots.activeSoundSystem;
    if(soundSystem)
    {
        source->position = crank_talk_oop_fromFloat(primContext->context, 0.0);
        if(!crank_talk_oop_asBoolean(primContext->context, source->isPlaying))
        {
            assert(!source->nextPlayingSource);
            source->isPlaying = crank_talk_oop_fromBoolean(primContext->context, true);
            if(soundSystem->lastPlayingSource)
            {
                soundSystem->lastPlayingSource->nextPlayingSource = source;
                soundSystem->lastPlayingSource = source;
            }
            else
            {
                soundSystem->firstPlayingSource = soundSystem->lastPlayingSource = source;
            }
        }
    }
    crank_talk_Mutex_unlock(&primContext->context->soundSystemMutex);

    return crank_talk_Primitive_return(primContext, self);
}

void
crank_talk_registerSoundPrimitives(crank_talk_VMContext_t* context)
{
    crank_talk_createAndSetPrimitiveMethod(context, (crank_talk_oop_t)crank_talk_oop_getClass(context, context->roots.SoundSampleClass), "fromFileNamed:", 2, "SoundSample::fromFileNamed:", crank_talk_SoundSample_fromFileNamed_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.SoundSourceClass, "play", 1, "SoundSource::play", crank_talk_SoundSource_play_primitive);
}