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
    Stdio stdout nextPutAll: 'Mouse move '; print: anEvent position; lf.
].

OSWindowEventHandler ![
visitMouseWheelEvent: anEvent
    Stdio stdout nextPutAll: 'Mouse wheel'; lf.
].

OSWindowEventHandler ![
visitWindowAnimationUpdateEvent: anEvent
    anEvent window render.
    Stdio stdout nextPutAll: 'Window animation update'; lf.
].

OSWindowEventHandler ![
visitWindowExposeEvent: anEvent
    anEvent window render.
    Stdio stdout nextPutAll: 'Window expose'; lf.
].

OSWindowEventHandler ![
visitGameControllerAddedDeviceEvent: anEvent
    Stdio stdout nextPutAll: 'Game controller added'; lf.
].

OSWindowEventHandler ![
visitGameControllerRemovedDeviceEvent: anEvent
    Stdio stdout nextPutAll: 'Game controller removed'; lf.
].

OSWindowEventHandler ![
visitGameControllerAxisEvent: anEvent
    Stdio stdout print: anEvent deviceIndex; nextPutAll: ' controller axis '; print: anEvent axis; space; print: anEvent value; lf.
].

OSWindowEventHandler ![
visitGameControllerButtonPressEvent: anEvent
    Stdio stdout nextPutAll: 'Press '; print: anEvent deviceIndex; space; print: anEvent button; lf.
].

OSWindowEventHandler ![
visitGameControllerButtonReleaseEvent: anEvent
    Stdio stdout nextPutAll: 'Release '; print: anEvent deviceIndex; space; print: anEvent button; lf.
].
