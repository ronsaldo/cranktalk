
SoundSource ![
initialize
    super initialize.
    
    isLooped := false.
    isPlaying := false.
    leftGain := 1.0.
    rightGain := 1.0.
    position := 0.0.
].

SoundSource ![
isLooped
    ^ isLooped
].

SoundSource ![
isLooped: aBoolean
    isLooped := aBoolean
].

SoundSource ![
isPlaying
    ^ isPlaying
].

SoundSource ![
leftGain
    ^ leftGain
].

SoundSource ![
rightGain
    ^ rightGain
].

SoundSource ![
position
    ^ position
].

SoundSource ![
sample
    ^ sample
].

SoundSource ![
sample: newSample
    sample := newSample
].