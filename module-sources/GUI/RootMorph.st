Morph << #RootMorph
    slots: {#currentKeyboardFocus . #currentMouseFocus. #currentGameControllerFocus};
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
processKeyboardEvent: aKeyboardEvent
    currentKeyboardFocus ~~ nil ifTrue: [
        currentKeyboardFocus processEvent: aKeyboardEvent
    ].
].

RootMorph ![
setNewKeyboardFocus: newKeyboardFocus
    currentKeyboardFocus ~~ nil ifTrue: [
        currentKeyboardFocus lostKeyboardFocus
    ].

    currentKeyboardFocus := newKeyboardFocus.

    currentKeyboardFocus ~~ nil ifTrue: [
        currentKeyboardFocus gotKeyboardFocus
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

RootMorph ![
getGameControllerFocus
    ^ currentGameControllerFocus
].

RootMorph ![
processGameControllerEvent: aGameControllerEvent
    currentGameControllerFocus ~~ nil ifTrue: [
        currentGameControllerFocus processEvent: aGameControllerEvent
    ].
].

RootMorph ![
setNewGameControllerFocus: newGameContollerFocus
    currentGameControllerFocus ~~ nil ifTrue: [
        currentGameControllerFocus lostGameControllerFocus
    ].

    currentGameControllerFocus := newGameContollerFocus.

    currentGameControllerFocus ~~ nil ifTrue: [
        currentGameControllerFocus gotGameControllerFocus
    ].
].