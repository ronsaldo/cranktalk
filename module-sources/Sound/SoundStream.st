SoundStream ![
isLooped
    ^ isLooped
].

SoundStream ![
isLooped: aBoolean
    isLooped := aBoolean
].

SoundStream ![
gain: aNumber
    leftGain := aNumber.
    rightGain := aNumber.
].
