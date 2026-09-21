Object << #Morph
    slots: {#bounds . #owner . #submorphs . #color. #layout};
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
processEvent: anEvent
    anEvent sentToMorph: self
].

Morph ![
handleUnknownEvent: anEvent
].

Morph ![
handleKeyboardDownEvent: anEvent
    Stdio stdout nextPutAll: 'handleKeyboardDownEvent '; print: anEvent symbol; lf
].

Morph ![
handleKeyboardUpEvent: anEvent
    Stdio stdout nextPutAll: 'handleKeyboardUpEvent '; print: anEvent symbol; lf
].

Morph ![
handleMouseMotionEvent: anEvent
    Stdio stdout nextPutAll: 'handleMouseMotionEvent '; print: anEvent position; space; print: anEvent delta; space; print: anEvent buttonState; lf
].

Morph ![
handleMouseButtonDownEvent: anEvent
    Stdio stdout nextPutAll: 'handleMouseButtonDownEvent '; print: anEvent position; space; print: anEvent buttonIndex; lf
].

Morph ![
handleMouseButtonUpEvent: anEvent
    Stdio stdout nextPutAll: 'handleMouseButtonUpEvent '; print: anEvent position; space; print: anEvent buttonIndex; lf
].

Morph ![
handleMouseClickEvent: anEvent
    Stdio stdout nextPutAll: 'handleMouseClickEvent '; print: anEvent position; space; print: anEvent buttonIndex; lf
].

Morph ![
handleMouseDoubleClickEvent: anEvent
    Stdio stdout nextPutAll: 'handleMouseDoubleClickEvent '; print: anEvent position; space; print: anEvent buttonIndex; lf
].

Morph ![
handleMouseWheelEvent: anEvent
    Stdio stdout nextPutAll: 'handleMouseWheelEvent '; print: anEvent position; space; print: anEvent scrollAmount; lf
].