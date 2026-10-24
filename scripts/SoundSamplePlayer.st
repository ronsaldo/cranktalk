| soundSample soundSource window |
soundSample := SoundSample fromFileNamed: 'assets/pong/hit.wav'.
soundSource := soundSample asSoundSource.

window := SimpleButtonMorph new
    label: 'Play sample';
    onClickAction: [:button |
        soundSource play
    ];
    openInWindow.

SoundSystem activate.
OSWindow mainLoop.
