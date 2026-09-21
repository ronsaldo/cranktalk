SimpleButtonMorph new
    label: 'Hello World';
    onClickAction: [:button |
        Stdio stdout nextPutAll: 'Hello World Clicked'; lf
    ];
    openInWindow.

OSWindow mainLoop.