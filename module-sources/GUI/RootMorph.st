Morph << #RootMorph
    slots: {#currentKeyboardFocus . #currentMouseFocus};
    build.

RootMorph ![
rootMorph
    ^ self
].

RootMorph ![
getKeyboardFocus
    ^ currentKeyboardFocus
].

RootMorph ![
setNewKeyboardFocus: newKeyboardFocus
    currentKeyboardFocus ~~ nil ifTrue: [
        currentKeyboardFocus lostKeyboardFocus
    ].

    currentKeyboardFocus := newKeyboardFocus.

    currentKeyboardFocus ~~ nil ifTrue: [
        currentKeyboardFocus gotKeybordFocus
    ].
].

RootMorph ![
getMouseFocus
    ^ currentMouseFocus
].

RootMorph ![
setNewMouseFocus: newMouseFocus
    currentMouseFocus ~~ nil ifTrue: [
        currentMouseFocus lostMouseFocus
    ].

    currentMouseFocus := newMouseFocus.

    currentMouseFocus ~~ nil ifTrue: [
        currentMouseFocus gotMouseFocus
    ].
].