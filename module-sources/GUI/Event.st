Object << #Event
    slots: {};
    build.

Event << #KeyboardEvent
    slots: {#isKeyRepeat . #keySymbol . #scanCode};
    build.

KeyboardEvent << #KeyboardDownEvent
    slots: {};
    build.

KeyboardEvent << #KeyboardUpEvent
    slots: {};
    build.

Event << #MouseEvent
    slots: {#position};
    build.

MouseEvent << #MouseButtonEvent
    slots: {#buttonIndex};
    build.

MouseButtonEvent << #MouseButtonDownEvent
    slots: {};
    build.

MouseButtonEvent << #MouseButtonUpEvent
    slots: {};
    build.

MouseButtonEvent << #MouseClickEvent
    slots: {};
    build.

MouseButtonEvent << #MouseDoubleClickEvent
    slots: {};
    build.

MouseEvent << #MouseMotionEvent
    slots: {#buttonState . #delta};
    build.
    
MouseEvent << #MouseWheelEvent
    slots: {#scrollAmount};
    build.

Event ![
sentToMorph: morph
    ^morph handleUnknownEvent: self
].

KeyboardDownEvent ![
sentToMorph: morph
    ^morph handleKeyboardDownEvent: self
].

KeyboardUpEvent ![
sentToMorph: morph
    ^morph handleKeyboardUpEvent: self
].

"Mouse Event"
MouseEvent ![
position
    ^ position
].

MouseEvent ![
position: aPoint
    position := aPoint
].

"MouseButtonEvent"
MouseButtonEvent ![
buttonIndex
    ^ buttonIndex
].

MouseButtonEvent ![
buttonIndex: anIndex
    buttonIndex := anIndex
].


"MouseButtonDownEvent"
MouseButtonDownEvent ![
sentToMorph: morph
    ^morph handleMouseButtonDownEvent: self
].

"MouseButtonUpEvent"
MouseButtonUpEvent ![
sentToMorph: morph
    ^morph handleMouseButtonUpEvent: self
].

MouseClickEvent ![
sentToMorph: morph
    ^morph handleMouseClickEvent: self
].

MouseDoubleClickEvent ![
sentToMorph: morph
    ^morph handleMouseDoubleClickEvent: self
].

"MouseMotionEvent"
MouseMotionEvent ![
sentToMorph: morph
    ^morph handleMouseMotionEvent: self
].

MouseMotionEvent ![
buttonState
    ^ buttonState
].

MouseMotionEvent ![
buttonState: anInteger
    buttonState := anInteger
].

MouseMotionEvent ![
delta
    ^ delta
].

MouseMotionEvent ![
delta: aPoint
    delta := aPoint
].

"MouseWheelEvent"
MouseWheelEvent ![
sentToMorph: morph
    ^morph handleMouseWheelEvent: self
].

MouseWheelEvent ![
scrollAmount
    ^ scrollAmount
].

MouseWheelEvent ![
scrollAmount: aPoint
    scrollAmount := aPoint
].
