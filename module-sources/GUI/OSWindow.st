OSWindow ![
initialize
    super initialize.
    extent := 640@480.
    eventHandler := OSWindowEventHandler new.
].

OSWindow ![
render
    Stdio stdout nextPutAll: 'Todo: Render OSWindow'; lf
].
