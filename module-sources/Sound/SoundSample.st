SoundSample ![
duration
    ^ duration
].

SoundSample ![
sampleCount
    ^ sampleCount
].

SoundSample ![
sampleRate
    ^ sampleRate
].

SoundSample ![
leftSamples
    ^ leftSamples
].

SoundSample ![
rightSamples
    ^ rightSamples
].

SoundSample ![
asSoundSource
    ^ SoundSource new sample: self; yourself
].