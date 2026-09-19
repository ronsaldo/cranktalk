"OSEvent"
OSEvent ![
window
    ^ window
].

"OSKeyboardEvent"
OSKeyboardEvent ![
scancode
    ^ scancode
].

OSKeyboardEvent ![
symbol
    ^ symbol
].

OSKeyboardEvent ![
modifiers
    ^ modifiers
].

OSKeyboardEvent ![
repeat
    ^ repeat
].

"OSKeyDownEvent"
OSKeyDownEvent ![
accept: anEventHandler
    ^ anEventHandler visitKeyDownEvent: self
].

"OSKeyUpEvent"
OSKeyUpEvent ![
accept: anEventHandler
    ^ anEventHandler visitKeyUpEvent: self
].

"OSTextInputEvent"
OSTextInputEvent ![
accept: anEventHandler
    ^ anEventHandler visitTextInputEvent: self
].

OSTextInputEvent ![
text
    ^ text
].

OSTextInputEvent ![
modifiers
    ^ modifiers
].

"OSMouseEvent"
OSMouseEvent ![
position
    ^ position
].

OSMouseEvent ![
delta
    ^ delta
].

OSMouseEvent ![
modifiers
    ^ modifiers
].

"OSMouseButtonEvent"
OSMouseButtonEvent ![
button
    ^ button
].

"OSMouseButtonPressEvent"
OSMouseButtonPressEvent ![
accept: anEventHandler
    ^ anEventHandler visitMouseButtonPressEvent: self
].

"OSMouseButtonReleaseEvent"
OSMouseButtonReleaseEvent ![
accept: anEventHandler
    ^ anEventHandler visitMouseButtonReleaseEvent: self
].

"OSMouseMoveEvent"
OSMouseMoveEvent ![
accept: anEventHandler
    ^ anEventHandler visitMouseMoveEvent: self
].

OSMouseMoveEvent ![
buttonState
    ^ buttonState
].

"OSMouseWheelEvent"
OSMouseWheelEvent ![
accept: anEventHandler
    ^ anEventHandler visitMouseWheelEvent: self
].

OSMouseWheelEvent ![
scrollHorizontal
    ^ scrollHorizontal
].

OSMouseWheelEvent ![
scrollVertical
    ^ scrollVertical
].

"OSWindowExposeEvent"
OSWindowExposeEvent ![
accept: anEventHandler
    ^ anEventHandler visitWindowExposeEvent: self
].