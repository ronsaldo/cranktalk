OSMouseButtonPressEvent ![
accept: anEventHandler
    ^ anEventHandler visitMouseButtonPressEvent: self
].

OSMouseButtonReleaseEvent ![
accept: anEventHandler
    ^ anEventHandler visitMouseButtonReleaseEvent: self
].

OSMouseMoveEvent ![
accept: anEventHandler
    ^ anEventHandler visitMouseMoveEvent: self
].

OSWindowExposeEvent ![
accept: anEventHandler
    ^ anEventHandler visitWindowExposeEvent: self
].