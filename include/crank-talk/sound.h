#ifndef CRANK_TALK_SOUND_H
#define CRANK_TALK_SOUND_H

#include "object-model.h"

typedef struct crank_talk_SoundSample_s crank_talk_SoundSample_t;
typedef struct crank_talk_SoundSystem_s crank_talk_SoundSystem_t;

struct crank_talk_SoundSample_s
{
    crank_talk_Object_t super;
    crank_talk_SoundSample_t *nextPlayingSample;
    crank_talk_oop_t isLooped;
    crank_talk_oop_t isPlaying;
    crank_talk_oop_t position;
    crank_talk_oop_t sampleCount;
    crank_talk_oop_t sampleRate;
    crank_talk_ByteArray_t *samples;
};

struct crank_talk_SoundSystem_s
{
    crank_talk_Object_t super;
    crank_talk_SoundSample_t *firstPlayingSample;
    crank_talk_SoundSample_t *lastPlayingSample;
    crank_talk_oop_t sampleRate;
};

#endif //CRANK_TALK_SOUND_H