BorderedMorph << #TextInputMorph
    slots: {#text};
    build.

TextInputMorph ![
initialize
    super initialize.
    text := ''.
].

TextInputMorph ![
defaultBounds
    ^ 0@0 corner: 200@30
].

TextInputMorph ![
defaultColor
    ^ Color white    
].

TextInputMorph ![
defaultTextColor
    ^ Color black    
].

TextInputMorph ![
drawOn: aCanvas
    super drawOn: aCanvas.
    aCanvas drawString: text inRectangle: self localBounds color: self defaultTextColor
].

TextInputMorph ![
onMouseButtonDownEvent: anEvent
    anEvent wasHandled ifTrue: [^ self].

    self takeKeyboardFocus.
    anEvent wasHandled: true
].

TextInputMorph ![
onTextInputEvent: anEvent
    text := text , anEvent text.
    self changed.
].
