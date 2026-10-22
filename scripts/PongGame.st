"Pong game entity"
Object << #PongGameEntity
    slots: {#position . #halfExtent};
    build.

PongGameEntity ![
initialize
    super initialize.
    position := 0@0.
    halfExtent := 1@1.
].

"Paddle"
PongGameEntity << #PongGamePaddle
    slots: {};
    build.

"Ball"
PongGameEntity << #PongGameBall
    slots: {};
    build.

"Game state"
Object << #PongGameState
    slots: {#firstPaddle. #secondPaddle. #ball};
    build.

PongGameState ![
initialize
    super initialize.
    firstPaddle := PongGamePaddle new.
    secondPaddle := PongGamePaddle new.
    ball := PongGameBall new.
].

PongGameState ![
firstPaddle
    ^ firstPaddle
].

PongGameState ![
secondPaddle
    ^ secondPaddle
].

PongGameState ![
ball
    ^ ball
].

"Game morph"
Morph << #PongGameMorph
    slots: {#gameState};
    build.

PongGameMorph ![
initialize
    super initialize.
    gameState := PongGameState new.
].

PongGameMorph ![
defaultBounds
    ^ 0@0 corner: 640@360
].

PongGameMorph ![
defaultColor
    ^ Color black
].

PongGameMorph ![
drawOn: aCanvas
    super drawOn: aCanvas
].

PongGameMorph new openInWindow.
OSWindow mainLoop.