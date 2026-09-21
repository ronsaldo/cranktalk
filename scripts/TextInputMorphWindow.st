| window 
|
window := TextInputMorph new
    onCancelAction: [:textInput |
        window close
    ];
    onEnterAction: [:textInput |
        Stdio stdout nextPutAll: textInput text; lf.
    ];
    openInWindow.

OSWindow mainLoop.