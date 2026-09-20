OSWindow ![
initialize
    super initialize.
    extent := 640@480.
    eventHandler := OSWindowEventHandler new.
].

OSWindow ![
extent
    ^ extent
].

OSWindow ![
extent: anExtent
    extent := anExtent
].

OSWindow ![
eventHandler
    ^ eventHandler
].

OSWindow ![
eventHandler: anEventHandler
    eventHandler := anEventHandler
].

OSWindow ![
render
    Stdio stdout nextPutAll: 'Todo: Render OSWindow'; lf
].
