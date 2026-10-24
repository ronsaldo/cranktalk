| soundSample window |
soundSample := SoundSample fromFileNamed: 'assets/pong/hit.wav'.
Stdio stdout print: soundSample; space; print: soundSample duration; space; print: soundSample position; lf.

window := SimpleButtonMorph new
    label: 'Play sample';
    onClickAction: [:button |
        Stdio stdout nextPutAll: 'TODO: play sample'; lf.
    ];
    openInWindow.

SoundSystem activate.
OSWindow mainLoop.
