#include "crank-talk/sound.h"
#include "crank-talk/context.h"
#include "crank-talk/message.h"
#include "crank-talk/utils.h"
#include <stdio.h>
#define SDL_MAIN_HANDLED
#include <SDL.h>

static int crank_talk_audioDeviceOpenCount = 0;
static SDL_AudioDeviceID crank_talk_openAudioDevice = 0;
static SDL_AudioSpec crank_talk_openAudioDeviceSpec;
void crank_talk_ensureSDL2Initialization(void);


void
crank_talk_SDL2ShutdownAudio(crank_talk_VMContext_t* context)
{
    if(crank_talk_audioDeviceOpenCount == 0)
        return;

    if(crank_talk_openAudioDevice != 0)
    {
        SDL_CloseAudioDevice(crank_talk_openAudioDevice);
        crank_talk_openAudioDevice = 0 ;
    }
    crank_talk_audioDeviceOpenCount = 0;
}

static void
crank_talk_MixSoundSourceChannel(crank_talk_VMContext_t *context,
    size_t bufferSize, float targetSamplingRate, float *destinationChannel,
    bool isLooped, float gain, float sourceSamplingRate, float sourceDuration, float sourcePosition, int sourceSampleCount, const float *sourceChannel)
{
    float dt = 1.0f/targetSamplingRate;
    float currentPosition = sourcePosition;
    for(size_t i = 0; i < bufferSize; ++i)
    {
        float sample = 0.0f;
        if(currentPosition >= sourceDuration)
        {
            if(isLooped)
                currentPosition = crank_talk_floorMod(currentPosition, sourceDuration);
        }

        if(currentPosition <= sourceDuration)
        {
            float samplePosition = currentPosition*sourceSamplingRate;
            float samplePositionFloor = floor(samplePosition);
            float samplePositionFract = samplePosition - samplePositionFloor;
            int sampleStartIndex = samplePositionFloor;
            int sampleEndIndex = (sampleStartIndex + 1) % sourceSampleCount;
            assert(sampleStartIndex < sourceSampleCount);
            assert(sampleEndIndex < sourceSampleCount);

            float startSample = sourceChannel[sampleStartIndex];
            float endSample = sourceChannel[sampleEndIndex];
            sample = crank_talk_fmix(startSample, endSample, samplePositionFract);
        }

        destinationChannel[i] += sample*gain;

        currentPosition = currentPosition + dt;
    }
}

static void
crank_talk_MixSoundSource(crank_talk_VMContext_t *context, crank_talk_SoundSource_t *soundSource, size_t bufferSize)
{
    if(!soundSource->sample)
        return;

    int sampleCount = crank_talk_oop_decodeSmallInteger(soundSource->sample->sampleCount);
    int sampleRate = crank_talk_oop_decodeSmallInteger(soundSource->sample->sampleRate);
    float duration = crank_talk_oop_numberAsFloat(soundSource->sample->duration);
    bool isLooped = crank_talk_oop_asBoolean(context, soundSource->isLooped);

    float bufferDuration = (float)bufferSize / crank_talk_openAudioDeviceSpec.freq;
    float sourceDuration = crank_talk_oop_numberAsFloat(soundSource->sample->duration);
    float bufferPosition = crank_talk_oop_numberAsFloat(soundSource->position);
    float leftGain = crank_talk_oop_numberAsFloat(soundSource->leftGain);
    float rightGain = crank_talk_oop_numberAsFloat(soundSource->rightGain);

    // Mix the channel
    crank_talk_MixSoundSourceChannel(context, bufferSize, crank_talk_openAudioDeviceSpec.freq, context->leftSoundChannelBuffer,
        isLooped, leftGain, sampleRate, sourceDuration, bufferPosition, sampleCount, (float*)soundSource->sample->leftSamples->elements);
    crank_talk_MixSoundSourceChannel(context, bufferSize, crank_talk_openAudioDeviceSpec.freq, context->rightSoundChannelBuffer,
        isLooped, rightGain, sampleRate, sourceDuration, bufferPosition, sampleCount, (float*)soundSource->sample->rightSamples->elements);


    // Advance the position.
    float newBufferPosition = bufferPosition + bufferDuration;
    soundSource->position = crank_talk_oop_fromFloat(context, newBufferPosition);
    if(newBufferPosition >= duration)
    {
        if(isLooped)
            newBufferPosition = crank_talk_floorMod(newBufferPosition, sourceDuration);
        else
            soundSource->isPlaying = crank_talk_oop_fromBoolean(context, false);
    }
}

static void
crank_talk_removeNonActiveSources(crank_talk_VMContext_t *context, crank_talk_SoundSystem_t *soundSystem)
{
    crank_talk_SoundSource_t *currentSoundSource = soundSystem->firstPlayingSource;
    soundSystem->firstPlayingSource = crank_talk_oop_nil();
    soundSystem->lastPlayingSource = crank_talk_oop_nil();

    while (currentSoundSource)
    {
        crank_talk_SoundSource_t *toAdd = currentSoundSource;
        currentSoundSource = currentSoundSource->nextPlayingSource;
        toAdd->nextPlayingSource = crank_talk_oop_nil();

        if(crank_talk_oop_asBoolean(context, toAdd->isPlaying))
        {
            if(soundSystem->lastPlayingSource)
            {
                soundSystem->lastPlayingSource->nextPlayingSource = toAdd;
                soundSystem->lastPlayingSource = toAdd;
            }
            else
            {
                soundSystem->firstPlayingSource = soundSystem->lastPlayingSource = toAdd;
            }
        }

    }
    
}

static void
crank_talk_SDL2AudioCallback(void *userdata, Uint8 * stream, int len)
{
    crank_talk_VMContext_t *context = userdata;
    size_t bufferSize = context->soundBufferSize;
    assert(crank_talk_openAudioDeviceSpec.samples == bufferSize);
    assert(len == bufferSize*2*2);

    // Clear the buffers
    memset(context->leftSoundChannelBuffer, 0, bufferSize *sizeof(float));
    memset(context->rightSoundChannelBuffer, 0, bufferSize *sizeof(float));

    // Fill the buffers
    {
        crank_talk_Mutex_lock(&context->soundSystemMutex);
        crank_talk_SoundSystem_t *soundSystem = (crank_talk_SoundSystem_t*)context->roots.activeSoundSystem;
        {
            crank_talk_SoundSource_t *currentSoundSource = soundSystem->firstPlayingSource;
            while (currentSoundSource)
            {
                crank_talk_MixSoundSource(context, currentSoundSource, bufferSize);
                currentSoundSource = currentSoundSource->nextPlayingSource;
            }
        }

        crank_talk_removeNonActiveSources(context, soundSystem);

        /*for(size_t i = 0; i < bufferSize; ++i)
        {
            float sample = (float)rand() / RAND_MAX*2.0 - 1.0;
            float gain = 0.01f;
            context->leftSoundChannelBuffer[i] += sample*gain;
            context->rightSoundChannelBuffer[i] += sample*gain;
        }*/

        crank_talk_Mutex_unlock(&context->soundSystemMutex);
    }

    // Encode the buffers
    int16_t *s16Stream = (int16_t *)stream;
    for(size_t i = 0; i < bufferSize; ++i)
    {
        float leftSample = crank_talk_fclamp(context->leftSoundChannelBuffer[i], -1, 1);
        float rightSample = crank_talk_fclamp(context->rightSoundChannelBuffer[i], -1, 1);

        s16Stream[i*2] = leftSample * INT16_MAX;
        s16Stream[i*2 + 1] = rightSample * INT16_MAX;
    }
}

static void
crank_talk_SoundSystem_activate_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 1 ) return crank_talk_Primitive_fail(primContext);

    if(crank_talk_audioDeviceOpenCount == 0)
    {
        primContext->context->roots.activeSoundSystem = CrankAllocateValuesObjectWithClassName(primContext->context, SoundSystem);

        crank_talk_ensureSDL2Initialization();
        SDL_AudioSpec desiredSpec = {
            .freq = 48000,
            .format = AUDIO_S16,
            .channels = 2,
            .samples = 1024,
            .callback = crank_talk_SDL2AudioCallback,
            .userdata = primContext->context,
        };

        crank_talk_openAudioDevice = SDL_OpenAudioDevice(NULL, 0, &desiredSpec, &crank_talk_openAudioDeviceSpec, 0);
        if(crank_talk_openAudioDevice != 0)
        {
            ++crank_talk_audioDeviceOpenCount;

            primContext->context->soundBufferSize = crank_talk_openAudioDeviceSpec.samples;
            primContext->context->leftSoundChannelBuffer = realloc(primContext->context->leftSoundChannelBuffer, sizeof(float)*primContext->context->soundBufferSize);
            primContext->context->rightSoundChannelBuffer = realloc(primContext->context->rightSoundChannelBuffer, sizeof(float)*primContext->context->soundBufferSize);
    
            SDL_PauseAudioDevice(crank_talk_openAudioDevice, 0);
        }
    }

    crank_talk_oop_t self = primContext->arguments[0];
    return crank_talk_Primitive_return(primContext, self);
}

static void
crank_talk_SoundSystem_deactivate_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 1 ) return crank_talk_Primitive_fail(primContext);
    if(crank_talk_audioDeviceOpenCount > 0)
    {
        --crank_talk_audioDeviceOpenCount;
        if(crank_talk_audioDeviceOpenCount == 0)
            crank_talk_SDL2ShutdownAudio(primContext->context);
    }

    crank_talk_oop_t self = primContext->arguments[0];
    return crank_talk_Primitive_return(primContext, self);
}

static void
crank_talk_SoundSystem_current_primitive(crank_talk_PrimitiveContext_t *primContext)
{
    if(primContext->argumentCount < 1 ) return crank_talk_Primitive_fail(primContext);
    return crank_talk_Primitive_return(primContext, primContext->context->roots.activeSoundSystem);
}

void
crank_talk_registerSoundBackendPrimitives(crank_talk_VMContext_t* context)
{
    crank_talk_createAndSetPrimitiveMethod(context, (crank_talk_oop_t)crank_talk_oop_getClass(context, context->roots.SoundSystemClass), "activate", 1, "SoundSystem::activate", crank_talk_SoundSystem_activate_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, (crank_talk_oop_t)crank_talk_oop_getClass(context, context->roots.SoundSystemClass), "deactivate", 1, "SoundSystem::deactivate", crank_talk_SoundSystem_deactivate_primitive);
 
    crank_talk_createAndSetPrimitiveMethod(context, (crank_talk_oop_t)crank_talk_oop_getClass(context, context->roots.SoundSystemClass), "current", 1, "SoundSystem::current", crank_talk_SoundSystem_current_primitive);       
}
