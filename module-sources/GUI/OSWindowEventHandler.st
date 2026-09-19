OSWindowEventHandler ![
processEvent: anEvent
    ^ anEvent accept: self
].

OSWindowEventHandler ![
visitKeyDownEvent: anEvent
    Stdio stdout nextPutAll: 'Key down '; print: anEvent symbol; lf.
].

OSWindowEventHandler ![
visitKeyUpEvent: anEvent
    Stdio stdout nextPutAll: 'Key up '; print: anEvent symbol; lf.
].

OSWindowEventHandler ![
visitTextInputEvent: anEvent
    Stdio stdout nextPutAll: 'Text input '; nextPutAll: anEvent text; lf.
].

OSWindowEventHandler ![
visitMouseButtonPressEvent: anEvent
    Stdio stdout nextPutAll: 'Mouse button press'; lf.
].

OSWindowEventHandler ![
visitMouseButtonReleaseEvent: anEvent
    Stdio stdout nextPutAll: 'Mouse button release'; lf.
].

OSWindowEventHandler ![
visitMouseMoveEvent: anEvent
    Stdio stdout nextPutAll: 'Mouse move'; lf.
].

OSWindowEventHandler ![
visitMouseWheelEvent: anEvent
    Stdio stdout nextPutAll: 'Mouse wheel'; lf.
].

OSWindowEventHandler ![
visitWindowExposeEvent: anEvent
    anEvent window render.
    Stdio stdout nextPutAll: 'Window expose'; lf.
].
