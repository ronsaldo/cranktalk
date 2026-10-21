BorderedMorph << #TextInputMorph
    slots: {#text. #cursorIndex. #onEnterAction. #onCancelAction};
    build.

TextInputMorph ![
initialize
    super initialize.
    text := ''.
    cursorIndex := 1.
    onCancelAction := [:x].
    onEnterAction := [:x].
].

TextInputMorph ![
defaultBounds
    ^ 0@0 corner: 200@20
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
    | cursorExtent cursorBounds |
    super drawOn: aCanvas.
    cursorExtent := (FontFace default measureTextExtent: text from: 1 to: cursorIndex).
    cursorBounds := self origin + (cursorExtent x @ 0) extent: 2@ self extent y.

    aCanvas fillRectangle: cursorBounds color: Color blue.
    aCanvas drawString: text inRectangle: self localBounds color: self defaultTextColor
].

TextInputMorph ![
text
    ^ text
].

TextInputMorph ![
text: aString
    text := aString
].

TextInputMorph ![
onCancelAction
    ^ onCancelAction
].

TextInputMorph ![
onCancelAction: aBlock
    onCancelAction := aBlock
].

TextInputMorph ![
onEnterAction
    ^ onEnterAction
].

TextInputMorph ![
onEnterAction: aBlock
    onEnterAction := aBlock
].

TextInputMorph ![
onMouseButtonDownEvent: anEvent
    anEvent wasHandled ifTrue: [^ self].

    self takeKeyboardFocus.
    anEvent wasHandled: true
].

TextInputMorph ![
onKeyboardDownEvent: anEvent
    anEvent wasHandled ifTrue: [^ self].

    anEvent symbol == OSK_RETURN ifTrue: [
        ^ onEnterAction value: self
    ].

    anEvent symbol == OSK_ESCAPE ifTrue: [
        ^ onCancelAction value: self
    ].

    anEvent symbol == OSK_BACKSPACE ifTrue: [
        (cursorIndex > 1 and: [text size >= 1]) ifTrue: [
            cursorIndex := cursorIndex - 1.
            text := text withoutElementAtIndex: cursorIndex.
        ]
    ].

    anEvent symbol == OSK_DELETE ifTrue: [
        cursorIndex <= text size ifTrue: [
            text := text withoutElementAtIndex: cursorIndex.
        ]
    ].

    anEvent symbol == OSK_LEFT ifTrue: [
        "Stdio stdout nextPutAll: 'Move cursor left'; lf."
        cursorIndex := cursorIndex - 1 max: 1.
    ].
    anEvent symbol == OSK_RIGHT ifTrue: [
        "Stdio stdout nextPutAll: 'Move cursor right'; lf."
        cursorIndex := cursorIndex + 1 min: text size + 1.
    ].

    anEvent symbol == OSK_HOME ifTrue: [
        "Stdio stdout nextPutAll: 'Move cursor begining'; lf."
        cursorIndex := 1.
    ].

    anEvent symbol == OSK_END ifTrue: [
        "Stdio stdout nextPutAll: 'Move cursor end'; lf."
        cursorIndex := text size + 1.
    ].

    "Stdio stdout print: anEvent symbol; lf."
    anEvent wasHandled: true.
    self changed
].

TextInputMorph ![
onKeyboardUpEvent: anEvent
    anEvent wasHandled ifTrue: [^ self].
    
    anEvent wasHandled: true.
].

TextInputMorph ![
onTextInputEvent: anEvent
    text := text insertString: anEvent text atIndex: cursorIndex.
    cursorIndex := cursorIndex + anEvent text size.
    self changed.
].
