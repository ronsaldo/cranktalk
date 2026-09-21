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

    anEvent symbol == 13 "Return" ifTrue: [
        ^ onEnterAction value: self
    ].

    anEvent symbol == 27 "Escape" ifTrue: [
        ^ onCancelAction value: self
    ].

    anEvent symbol == 8 "Backspace" ifTrue: [
        (cursorIndex > 1 and: [text size >= 1]) ifTrue: [
            cursorIndex := cursorIndex - 1.
            text := text withoutElementAtIndex: cursorIndex.
        ]
    ].

    anEvent symbol == 127 "Del" ifTrue: [
        cursorIndex <= text size ifTrue: [
            text := text withoutElementAtIndex: cursorIndex.
        ]
    ].

    anEvent symbol == 1073741904 "Left arrow" ifTrue: [
        "Stdio stdout nextPutAll: 'Move cursor left'; lf."
        cursorIndex := cursorIndex - 1 max: 1.
    ].
    anEvent symbol == 1073741903  "Right arrow" ifTrue: [
        "Stdio stdout nextPutAll: 'Move cursor right'; lf."
        cursorIndex := cursorIndex + 1 min: text size + 1.
    ].

    anEvent symbol == 1073741898  "Home" ifTrue: [
        "Stdio stdout nextPutAll: 'Move cursor begining'; lf."
        cursorIndex := 1.
    ].

    anEvent symbol == 1073741901  "End" ifTrue: [
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
    text := text , anEvent text.
    cursorIndex := cursorIndex + anEvent text size.
    self changed.
].
