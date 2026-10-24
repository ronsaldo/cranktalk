#include "crank-talk/sound.h"
#include "crank-talk/bytes.h"
#include "crank-talk/context.h"
#include "crank-talk/errors.h"
#include "crank-talk/io.h"
#include "crank-talk/message.h"
#include "crank-talk/string.h"
#include <stdlib.h>
#include <stdio.h>

#define STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.c"

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

static void
crank_talk_SoundStream_fromFileNamed_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 2) return crank_talk_Primitive_fail(primContext);

    crank_talk_oop_t fileNameValue = primContext->arguments[1];

    char *fileName = crank_talk_String_asCString(fileNameValue);
    crank_talk_ByteArray_t *memory = crank_talk_io_readWholeFileAsByteArray(primContext->context, fileName);
    free(fileName);

    size_t memorySize = crank_talk_object_getSizeInBytes((crank_talk_oop_t)memory);
    int error = 0;
    stb_vorbis *vorbisDecoder = stb_vorbis_open_memory(memory->elements, (int)memorySize, &error, NULL);
    if(!vorbisDecoder)
        crank_talk_errorPrintf(primContext->context, "Failed to open ogg vorbis decoder.");

    stb_vorbis_info vorbisInfo = stb_vorbis_get_info(vorbisDecoder);
    if(vorbisInfo.channels > 2)
    {
        stb_vorbis_close(vorbisDecoder);
        crank_talk_errorPrintf(primContext->context, "Unsupported OGG vorbis with more than 2 channels.");
    }

    crank_talk_SoundStream_t *soundStream = (crank_talk_SoundStream_t *)CrankAllocateValuesObjectWithClassName(primContext->context, SoundStream);
    soundStream->memory = memory;

    soundStream->channelCount = crank_talk_oop_encodeSmallInteger(vorbisInfo.channels);
    soundStream->sampleCount = crank_talk_oop_encodeSmallInteger(stb_vorbis_stream_length_in_samples(vorbisDecoder));
    soundStream->sampleRate = crank_talk_oop_encodeSmallInteger(vorbisInfo.sample_rate);
    soundStream->duration = crank_talk_oop_fromFloat(primContext->context, stb_vorbis_stream_length_in_seconds(vorbisDecoder));
    soundStream->position = crank_talk_oop_fromFloat(primContext->context, 0);

    soundStream->isLooped = crank_talk_oop_fromBoolean(primContext->context, false);
    soundStream->isPlaying = crank_talk_oop_fromBoolean(primContext->context, false);
    soundStream->leftGain = crank_talk_oop_fromFloat(primContext->context, 1.0);
    soundStream->rightGain = crank_talk_oop_fromFloat(primContext->context, 1.0);

    soundStream->vorbisDecoder = (crank_talk_ExternalAddress_t*)crank_talk_ExternalAddress_make(primContext->context, vorbisDecoder);

    if(vorbisInfo.channels == 1)
    {
        soundStream->leftBuffer = soundStream->rightBuffer = (crank_talk_ByteArray_t*)crank_talk_ByteArray_create(primContext->context, 1024*sizeof(float));
    }
    else
    {
        soundStream->leftBuffer = (crank_talk_ByteArray_t*)crank_talk_ByteArray_create(primContext->context, 1024*sizeof(float));
        soundStream->rightBuffer = (crank_talk_ByteArray_t*)crank_talk_ByteArray_create(primContext->context, 1024*sizeof(float));
    }
 
    return crank_talk_Primitive_return(primContext, (crank_talk_oop_t)soundStream);
}

static void
crank_talk_SoundStream_play_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 1) return crank_talk_Primitive_fail(primContext);

    crank_talk_SoundStream_t *stream  = (crank_talk_SoundStream_t *)primContext->arguments[0];

    crank_talk_Mutex_lock(&primContext->context->soundSystemMutex);
    crank_talk_SoundSystem_t *soundSystem = (crank_talk_SoundSystem_t *)primContext->context->roots.activeSoundSystem;
    if(soundSystem)
    {
        stream->position = crank_talk_oop_fromFloat(primContext->context, 0.0);
        if(!crank_talk_oop_asBoolean(primContext->context, stream->isPlaying))
        {
            assert(!stream->nextPlayingStream);
            stream->isPlaying = crank_talk_oop_fromBoolean(primContext->context, true);
            if(soundSystem->lastPlayingStream)
            {
                soundSystem->lastPlayingStream->nextPlayingStream = stream;
                soundSystem->lastPlayingStream = stream;
            }
            else
            {
                soundSystem->firstPlayingStream = soundSystem->lastPlayingStream = stream;
            }
        }
    }
    crank_talk_Mutex_unlock(&primContext->context->soundSystemMutex);

    return crank_talk_Primitive_return(primContext, (crank_talk_oop_t)stream);
}

static void
crank_talk_SoundStream_stop_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 1) return crank_talk_Primitive_fail(primContext);

    crank_talk_SoundStream_t *stream  = (crank_talk_SoundStream_t *)primContext->arguments[0];

    crank_talk_Mutex_lock(&primContext->context->soundSystemMutex);
    stream->isPlaying = crank_talk_oop_fromBoolean(primContext->context, false);
    if(stream->vorbisDecoder)
    {
        stb_vorbis_close(stream->vorbisDecoder->address);
        stream->vorbisDecoder = NULL;
    }
    crank_talk_Mutex_unlock(&primContext->context->soundSystemMutex);


    return crank_talk_Primitive_return(primContext, crank_talk_oop_nil());
}

void
crank_talk_registerSoundPrimitives(crank_talk_VMContext_t* context)
{
    crank_talk_createAndSetPrimitiveMethod(context, (crank_talk_oop_t)crank_talk_oop_getClass(context, context->roots.SoundSampleClass), "fromFileNamed:", 2, "SoundSample::fromFileNamed:", crank_talk_SoundSample_fromFileNamed_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.SoundSourceClass, "play", 1, "SoundSource::play", crank_talk_SoundSource_play_primitive);

    crank_talk_createAndSetPrimitiveMethod(context, (crank_talk_oop_t)crank_talk_oop_getClass(context, context->roots.SoundStreamClass), "fromFileNamed:", 2, "SoundStream::fromFileNamed:", crank_talk_SoundStream_fromFileNamed_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.SoundStreamClass, "play", 1, "SoundStream::play", crank_talk_SoundStream_play_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, context->roots.SoundStreamClass, "stop", 1, "SoundStream::stop", crank_talk_SoundStream_stop_primitive);

}