| window |
window := SimpleButtonMorph new
    label: 'Hello World';
    onClickAction: [:button |
        Stdio stdout nextPutAll: 'Hello World Clicked'; lf.
        window close
    ];
    openInWindow.

OSWindow mainLoop.