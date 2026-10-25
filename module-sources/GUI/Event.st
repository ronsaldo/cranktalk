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

Event << #WindowOpenEvent
    slots: {};
    build.

Event << #GameControllerEvent
    slots: {#deviceIndex};
    build.

GameControllerEvent << #GameControllerAddedEvent
    slots: {};
    build.

GameControllerEvent << #GameControllerRemovedEvent
    slots: {};
    build.

GameControllerEvent << #GameControllerAxisEvent
    slots: {#axis . #value};
    build.

GameControllerEvent << #GameControllerButtonEvent
    slots: {#button};
    build.

GameControllerButtonEvent << #GameControllerButtonPressEvent
    slots: {};
    build.

GameControllerButtonEvent << #GameControllerButtonReleaseEvent
    slots: {};
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

"WindowOpenEvent"
WindowOpenEvent ![
sentToMorph: morph
    ^morph handleWindowOpenEvent: self
].

"GameControllerEvent"
GameControllerEvent ![
deviceIndex
    ^ deviceIndex
].

GameControllerEvent ![
deviceIndex: anIndex
    deviceIndex := anIndex
].

"GameControlleAddedEvent"
GameControllerAddedEvent ![
sentToMorph: morph
    ^morph handleGameControllerAddedEvent: self
].

"GameControlleRemovedEvent"
GameControllerRemovedEvent ![
sentToMorph: morph
    ^morph handleGameControllerRemovedEvent: self
].

"GameControllerAxisEvent"
GameControllerAxisEvent ![
sentToMorph: morph
    ^morph handleGameControllerAxisEvent: self
].

GameControllerAxisEvent ![
axis
    ^ axis
].

GameControllerAxisEvent ![
axis: anInteger
    axis := anInteger
].

GameControllerAxisEvent ![
value
    ^ value
].

GameControllerAxisEvent ![
value: asFloat
    value := asFloat
].

GameControllerAxisEvent ![
valueWithoutDeadZone
    (-0.2 <= value and: [value <= 0.2]) ifTrue: [
        ^ 0.0
    ].
    ^ value
].

GameControllerAxisEvent ![
isLeftX
    ^ axis == 0
].

GameControllerAxisEvent ![
isLeftY
    ^ axis == 1
].

GameControllerAxisEvent ![
isRightX
    ^ axis == 2
].

GameControllerAxisEvent ![
isRightY
    ^ axis == 3
].

GameControllerAxisEvent ![
isLeftTrigger
    ^ axis == 4
].

GameControllerAxisEvent ![
isRightTrigger
    ^ axis == 5
].

"GameControllerButtonEvent"
GameControllerButtonEvent ![
button
    ^ button
].

GameControllerButtonEvent ![
button: anInteger
    button := anInteger
].

"GameControllerButtonPressEvent"
GameControllerButtonPressEvent ![
sentToMorph: morph
    ^morph handleGameControllerButtonPressEvent: self
].

"GameControllerButtonReleaseEvent"
GameControllerButtonReleaseEvent ![
sentToMorph: morph
    ^morph handleGameControllerButtonReleaseEvent: self
].

