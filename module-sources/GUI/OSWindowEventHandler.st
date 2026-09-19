OSWindowEventHandler ![
processEvent: anEvent
    ^ anEvent accept: self
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
    Stdio stdout nextPutAll: 'Window expose'; lf.
].
