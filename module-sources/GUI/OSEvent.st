"OSEvent"
OSEvent ![
window
    ^ window
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