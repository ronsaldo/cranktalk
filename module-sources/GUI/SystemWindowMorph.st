OSWindowEventHandler << #SystemWindowMorphEventHandler
    slots: {#systemWindowMorph};
    build.

RootMorph << #SystemWindowMorph
    slots: {#osWindow};
    build.

Morph ![
openInWindow
    ^ SystemWindowMorph new
        extent: self extent;
        addMorph: self;
        layout: (FillMorphicLayout new morph: self; yourself);
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
close
    osWindow close.
    osWindow := nil.
].
SystemWindowMorph ![
changed
    self render
].

SystemWindowMorph ![
render
    | canvas windowExtent formExtent scaleFactor|
    osWindow renderer beginFrame.
    windowExtent := osWindow extent.
    self extent: windowExtent.

    formExtent := osWindow renderer form extent.
    scaleFactor := formExtent // windowExtent.

    canvas := FormCanvas on: osWindow renderer form.
    canvas scaleFactor: scaleFactor.
    self fullDrawOn: canvas.

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
    systemWindowMorph processKeyboardEvent: (KeyboardDownEvent new
        isKeyRepeat: anEvent repeat > 1;
        scancode: anEvent scancode;
        symbol: anEvent symbol;
        yourself).
].

SystemWindowMorphEventHandler ![
visitKeyUpEvent: anEvent
    systemWindowMorph processKeyboardEvent: (KeyboardUpEvent new
        isKeyRepeat: anEvent repeat > 1;
        scancode: anEvent scancode;
        symbol: anEvent symbol;
        yourself).
].

SystemWindowMorphEventHandler ![
visitTextInputEvent: anEvent
    systemWindowMorph processKeyboardEvent: (TextInputEvent new
        text: anEvent text;
        yourself).
].

SystemWindowMorphEventHandler ![
visitMouseButtonPressEvent: anEvent
    systemWindowMorph processEvent: (MouseButtonDownEvent new
        position: anEvent position;
        buttonIndex: anEvent button;
        yourself).
    anEvent clicks = 1 ifTrue: [
        systemWindowMorph processEvent: (MouseClickEvent new
            position: anEvent position;
            buttonIndex: anEvent button;
            yourself).
    ].
    anEvent clicks = 2 ifTrue: [
        systemWindowMorph processEvent: (MouseDoubleClickEvent new
            position: anEvent position;
            buttonIndex: anEvent button;
            yourself).
    ].
].

SystemWindowMorphEventHandler ![
visitMouseButtonReleaseEvent: anEvent
    systemWindowMorph processEvent: (MouseButtonUpEvent new
        position: anEvent position;
        buttonIndex: anEvent button;
        yourself)
].

SystemWindowMorphEventHandler ![
visitMouseMoveEvent: anEvent
    systemWindowMorph processEvent: (MouseMotionEvent new
        position: anEvent position;
        delta: anEvent delta;
        buttonState: anEvent buttonState;
        yourself)
].

SystemWindowMorphEventHandler ![
visitMouseWheelEvent: anEvent
    systemWindowMorph processEvent: (MouseWheelEvent new
        position: anEvent position;
        scrollAmount: anEvent scrollHorizontal @ anEvent scrollVertical;
        yourself)
].

SystemWindowMorphEventHandler ![
visitWindowExposeEvent: anEvent
    systemWindowMorph render
].