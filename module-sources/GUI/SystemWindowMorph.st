OSWindowEventHandler << #SystemWindowMorphEventHandler
    slots: {#systemWindowMorph};
    build.

RootMorph << #SystemWindowMorph
    slots: {#osWindow};
    build.

Morph ![
openInWindow
    SystemWindowMorph new
        extent: self extent;
        addMorph: self;
        open
].

SystemWindowMorph ![
defaultColor
    ^ Color white
].

SystemWindowMorph ![
open
    osWindow := OSWindow new.
    osWindow
        extent: self extent;
        eventHandler: (SystemWindowMorphEventHandler new
            systemWindowMorph: self;
            yourself);
        open;
        newFormRenderer;
        yourself
].
SystemWindowMorph ![
render
    | canvas |
    osWindow renderer beginFrame.
    Stdio stdout print: osWindow extent; lf.
    
    canvas := FormCanvas on: osWindow renderer form.
    canvas fillWithColor: Color green.
    self fullDrawOn: canvas.
        "fillRectangle: (0@0 extent: 20@20) color: Color blue."

    osWindow renderer
        blitForm;
        endFrame
].

SystemWindowMorphEventHandler ![
systemWindowMorph: aSystemWindowMorph
    systemWindowMorph := aSystemWindowMorph
].

SystemWindowMorphEventHandler ![
visitKeyDownEvent: anEvent
    Stdio stdout nextPutAll: 'Morph Key down '; print: anEvent symbol; lf.
].

SystemWindowMorphEventHandler ![
visitKeyUpEvent: anEvent
    Stdio stdout nextPutAll: 'Morph Key up '; print: anEvent symbol; lf.
].

SystemWindowMorphEventHandler ![
visitTextInputEvent: anEvent
    Stdio stdout nextPutAll: 'Morph Text input '; nextPutAll: anEvent text; lf.
].

SystemWindowMorphEventHandler ![
visitMouseButtonPressEvent: anEvent
    Stdio stdout nextPutAll: 'Morph Mouse button press'; lf.
].

SystemWindowMorphEventHandler ![
visitMouseButtonReleaseEvent: anEvent
    Stdio stdout nextPutAll: 'Morph Mouse button release'; lf.
].

SystemWindowMorphEventHandler ![
visitMouseMoveEvent: anEvent
    Stdio stdout nextPutAll: 'Morph Mouse move '; print: anEvent position; lf.
].

SystemWindowMorphEventHandler ![
visitMouseWheelEvent: anEvent
    Stdio stdout nextPutAll: 'Morph Mouse wheel'; lf.
].

SystemWindowMorphEventHandler ![
visitWindowExposeEvent: anEvent
    systemWindowMorph render
].