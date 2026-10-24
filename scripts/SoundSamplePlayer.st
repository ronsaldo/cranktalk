| window |
window := SimpleButtonMorph new
    label: 'Play sample';
    onClickAction: [:button |
        Stdio stdout nextPutAll: 'TODO: play sample'; lf.
    ];
    openInWindow.

SoundSystem activate.
OSWindow mainLoop.
