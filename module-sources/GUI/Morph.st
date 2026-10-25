Object << #Morph
    slots: {#bounds . #owner . #submorphs . #color. #layout};
    sharedPools: {OSKeySymbols};
    build.

Morph ![
initialize
    super initialize.
    submorphs := OrderedCollection new.
    bounds := self defaultBounds.
    color := self defaultColor.
].

Morph ![
addMorph: aMorph
    self assert: aMorph owner == nil.
    submorphs add: aMorph.
    aMorph owner: self
].

Morph ![
defaultBounds
    ^ 0@0 corner: 50@40
].

Morph ![
defaultColor
    ^ Color blue
].

Morph ![
currentColor
    ^ color
].

Morph ![
bounds
    ^ bounds
].

Morph ![
bounds: newBounds
    bounds := newBounds.
    self updateLayout
].

Morph ![
localBounds
    ^ 0@0 corner: self extent
].

Morph ![
extent
    ^ bounds extent
].

Morph ![
extent: anExtent
    bounds := bounds origin extent: anExtent.
    self updateLayout.
].

Morph ![
origin
    ^ bounds origin
].

Morph ![
origin: newOrigin
    bounds := newOrigin extent: self extent
].

Morph ![
owner
    ^ owner
].

Morph ![
owner: anOwner
    owner := anOwner
].

Morph ![
fullDrawOn: aCanvas
    aCanvas withTranslation: bounds origin do: [
        self drawOn: aCanvas.
        self drawChildrenWith: aCanvas
    ]
].

Morph ![
drawOn: aCanvas
    aCanvas fillRectangle: self localBounds color: self currentColor
].

Morph ![
drawChildrenWith: aCanvas
    submorphs do: [:each |
        each fullDrawOn: aCanvas
    ]
].

Morph ![
fitSize
    "By default do nothing"
].

Morph ![
updateLayout
    layout ~~ nil ifTrue: [
        layout applyWithLocalBounds: self localBounds
    ]
].

Morph ![
layout
    ^ layout
].

Morph ![
layout: aLayout
    layout := aLayout.
    self updateLayout
].

Morph ![
rootMorph
    ^ owner ~~ nil ifTrue: [owner rootMorph] ifFalse: [nil]
].

Morph ![
getMouseFocus
    ^ self rootMorph getMouseFocus
].

Morph ![
hasMouseFocus
    ^ self getMouseFocus == self
].

Morph ![
gotMouseFocus
].

Morph ![
lostMouseFocus
].

Morph ![
takeMouseFocus
    ^ self rootMorph setNewMouseFocus: self
].

Morph ![
gotKeyboardFocus
].

Morph ![
lostKeyboardFocus
].

Morph ![
takeKeyboardFocus
    ^ self rootMorph setNewKeyboardFocus: self
].

Morph ![
gotGameControllerFocus
].

Morph ![
lostGameControllerFocus
].

Morph ![
takeGameControllerFocus
    ^ self rootMorph setNewGameControllerFocus: self
].

Morph ![
updateAnimation
    submorphs do: [:each | each updateAnimation]
].

Morph ![
changed
    self rootMorph changed
].

Morph ![
processEvent: anEvent
    anEvent sentToMorph: self
].

Morph ![
handleUnknownEvent: anEvent
].

Morph ![
handleKeyboardDownEvent: anEvent
    self onKeyboardDownEvent: anEvent.
].

Morph ![
handleKeyboardUpEvent: anEvent
    self onKeyboardUpEvent: anEvent.
].

Morph ![
handleTextInputEvent: anEvent
    self onTextInputEvent: anEvent.
].

Morph ![
handleMouseMotionEvent: anEvent
    anEvent withTranslationInverse: self origin do: [
        submorphs reverseDo: [:each |
            (each bounds containsPoint: anEvent position) ifTrue: [
                each processEvent: anEvent
            ]
        ].
        self onMouseMotionEvent: anEvent
    ].
].

Morph ![
handleMouseButtonDownEvent: anEvent
    anEvent withTranslationInverse: self origin do: [
        submorphs reverseDo: [:each |
            (each bounds containsPoint: anEvent position) ifTrue: [
                each processEvent: anEvent
            ]
        ].
        self onMouseButtonDownEvent: anEvent
    ].
].

Morph ![
handleMouseButtonUpEvent: anEvent
    anEvent withTranslationInverse: self origin do: [
        submorphs reverseDo: [:each |
            (each bounds containsPoint: anEvent position) ifTrue: [
                each processEvent: anEvent
            ]
        ].
        self onMouseButtonUpEvent: anEvent
    ].
].

Morph ![
handleMouseClickEvent: anEvent
    anEvent withTranslationInverse: self origin do: [
        submorphs reverseDo: [:each |
            (each bounds containsPoint: anEvent position) ifTrue: [
                each processEvent: anEvent
            ]
        ].
        self onMouseClickEvent: anEvent
    ].
].

Morph ![
handleMouseDoubleClickEvent: anEvent
    anEvent withTranslationInverse: self origin do: [
        submorphs reverseDo: [:each |
            (each bounds containsPoint: anEvent position) ifTrue: [
                each processEvent: anEvent
            ]
        ].
        self onMouseDoubleClickEvent: anEvent
    ].
].

Morph ![
handleMouseWheelEvent: anEvent
    anEvent withTranslationInverse: self origin do: [
        submorphs reverseDo: [:each |
            (each bounds containsPoint: anEvent position) ifTrue: [
                each processEvent: anEvent
            ]
        ].
        self onMouseWheelEvent: anEvent
    ].
].

Morph ![
handleWindowOpenEvent: anEvent
    self onWindowOpenEvent: anEvent.

    submorphs do: [:each |
        each processEvent: anEvent
    ].
].

Morph ![
handleGameControllerAddedEvent: anEvent
    self onGameControllerAddedEvent: anEvent
].

Morph ![
handleGameControllerRemovedEvent: anEvent
    self onGameControllerRemovedEvent: anEvent
].

Morph ![
handleGameControllerAxisEvent: anEvent
    self onGameControllerAxisEvent: anEvent
].

Morph ![
handleGameControllerButtonPressEvent: anEvent
    self onGameControllerButtonPressEvent: anEvent
].

Morph ![
handleGameControllerButtonReleaseEvent: anEvent
    self onGameControllerButtonReleaseEvent: anEvent
].

Morph ![
onKeyboardDownEvent: anEvent
    "Stdio stdout nextPutAll: 'onMouseMotionEvent '; print: anEvent position; space; print: anEvent delta; lf"
].

Morph ![
onKeyboardUpEvent: anEvent
    "Stdio stdout nextPutAll: 'onMouseMotionEvent '; print: anEvent position; space; print: anEvent delta; lf"
].

Morph ![
onTextInputEvent: anEvent
    "Stdio stdout nextPutAll: 'onMouseMotionEvent '; print: anEvent position; space; print: anEvent delta; lf"
].

Morph ![
onMouseMotionEvent: anEvent
    "Stdio stdout nextPutAll: 'onMouseMotionEvent '; print: anEvent position; space; print: anEvent delta; lf"
].

Morph ![
onMouseButtonDownEvent: anEvent
    "Stdio stdout nextPutAll: 'onMouseButtonDownEvent '; print: anEvent position; space; print: anEvent buttonIndex; lf"
].

Morph ![
onMouseButtonUpEvent: anEvent
    "Stdio stdout nextPutAll: 'onMouseButtonUpEvent '; print: anEvent position; space; print: anEvent buttonIndex; lf"
].

Morph ![
onMouseClickEvent: anEvent
    "Stdio stdout nextPutAll: 'onMouseClickEvent '; print: anEvent position; space; print: anEvent buttonIndex; lf"
].

Morph ![
onMouseDoubleClickEvent: anEvent
    "Stdio stdout nextPutAll: 'onMouseDoubleClickEvent '; print: anEvent position; space; print: anEvent buttonIndex; lf"
].

Morph ![
onMouseWheelEvent: anEvent
    "Stdio stdout nextPutAll: 'onMouseWheelEvent '; print: anEvent position; space; print: anEvent scrollAmount; lf"
].

Morph ![
onWindowOpenEvent: anEvent
    "Stdio stdout nextPutAll: 'onWindowOpenEvent'; lf"
].

Morph ![
onGameControllerAddedEvent: anEvent
    "Stdio stdout nextPutAll: 'onGameControllerAddedEvent '; print: anEvent deviceIndex; lf"
].

Morph ![
onGameControllerRemovedEvent: anEvent
    "Stdio stdout nextPutAll: 'onGameControllerRemovedEvent '; print: anEvent deviceIndex; lf"
].

Morph ![
onGameControllerAxisEvent: anEvent
    "Stdio stdout nextPutAll: 'onGameControllerAxisEvent '; print: anEvent axis; space; print: anEvent value; lf"
].

Morph ![
onGameControllerButtonPressEvent: anEvent
    "Stdio stdout nextPutAll: 'onGameControllerButtonPressEvent '; print: anEvent button; lf"
].

Morph ![
onGameControllerButtonReleaseEvent: anEvent
    "Stdio stdout nextPutAll: 'onGameControllerButtonReleaseEvent '; print: anEvent button; lf"
].