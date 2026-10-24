#include "crank-talk/sound.h"
#include "crank-talk/context.h"
#include "crank-talk/message.h"
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

    printf("TODO: crank_talk_SDL2ShutdownAudio\n");
    crank_talk_audioDeviceOpenCount = 0;
}

static void crank_talk_SDL2AudioCallback(void *userdata, Uint8 * stream, int len)
{
    memset(stream, 0, len);
    printf("crank_talk_SDL2AudioCallback len: %d\n", len);
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
        printf("crank_talk_openAudioDevice: %d\n", crank_talk_openAudioDevice);
        if(crank_talk_openAudioDevice != 0)
            SDL_PauseAudioDevice(crank_talk_openAudioDevice, 0);
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

void
crank_talk_registerSoundBackendPrimitives(crank_talk_VMContext_t* context)
{
    crank_talk_createAndSetPrimitiveMethod(context, (crank_talk_oop_t)crank_talk_oop_getClass(context, context->roots.SoundSystemClass), "activate", 1, "SoundSystem::activate", crank_talk_SoundSystem_activate_primitive);
    crank_talk_createAndSetPrimitiveMethod(context, (crank_talk_oop_t)crank_talk_oop_getClass(context, context->roots.SoundSystemClass), "deactivate", 1, "SoundSystem::deactivate", crank_talk_SoundSystem_deactivate_primitive);
}