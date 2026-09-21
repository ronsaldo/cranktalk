Object << #Event
    slots: {#wasHandled};
    build.

Event << #KeyboardEvent
    slots: {#isKeyRepeat . #symbol . #scancode};
    build.

KeyboardEvent << #KeyboardDownEvent
    slots: {};
    build.

KeyboardEvent << #KeyboardUpEvent
    slots: {};
    build.

Event << #TextInputEvent
    slots: {#text};
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

"Event"
Event ![
initialize
    super initialize.
    wasHandled := false.
].

Event ![
sentToMorph: morph
    ^morph handleUnknownEvent: self
].

Event ![
wasHandled
    ^ wasHandled
].

Event ![
wasHandled: aBoolean
    wasHandled := aBoolean
].

"KeyboardEvent"
KeyboardEvent ![
isKeyRepeat
    ^ isKeyRepeat
].

KeyboardEvent ![
isKeyRepeat: aBoolean
    isKeyRepeat := aBoolean
].

KeyboardEvent ![
symbol
    ^ symbol
].

KeyboardEvent ![
symbol: anInteger
    symbol := anInteger
].

KeyboardEvent ![
scancode
    ^ scancode
].

KeyboardEvent ![
scancode: anInteger
    scancode := anInteger
].

KeyboardDownEvent ![
sentToMorph: morph
    ^morph handleKeyboardDownEvent: self
].

KeyboardUpEvent ![
sentToMorph: morph
    ^morph handleKeyboardUpEvent: self
].

"TextInputEvent"
TextInputEvent ![
sentToMorph: morph
    ^morph handleTextInputEvent: self
].

TextInputEvent ![
text
    ^ text
].

TextInputEvent ![
text: aString
    text := aString
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

MouseEvent ![
withTranslationInverse: translation do: aBlock
    | oldPosition |
    oldPosition := position.
    position := position - translation.
    aBlock ensure: [
        position := oldPosition
    ]
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
