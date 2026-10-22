"Pong game entity"
Object << #PongGameEntity
    slots: {#position . #halfExtent. #color};
    build.

PongGameEntity ![
initialize
    super initialize.
    position := 0.0@0.0.
    halfExtent := 1.0@1.0.
    color := Color white.
].

PongGameEntity ![
bounds
    ^ position - halfExtent corner: position + halfExtent
].

PongGameEntity ![
position
    ^ position
].

PongGameEntity ![
position: newPosition
    position := newPosition
].

PongGameEntity ![
drawOn: aCanvas
    aCanvas fillRectangle: self bounds color: color
].

"Paddle"
PongGameEntity << #PongGamePaddle
    slots: {};
    build.

PongGamePaddle ![
initialize
    super initialize.
    halfExtent := 0.5@4.0
].

"Ball"
PongGameEntity << #PongGameBall
    slots: {};
    build.

PongGameBall ![
initialize
    super initialize.
    halfExtent := 0.5@0.5
].

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

    firstPaddle position: 1.0@12.0.
    secondPaddle position: 39.0@12.0.
    ball position: 20.0@12.0.
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
    | aspectRatio scaleFactor |
    super drawOn: aCanvas.

    scaleFactor := self extent / (40.0@25.0).
    aCanvas withScaleFactor: scaleFactor do: [
        gameState firstPaddle drawOn: aCanvas.
        gameState secondPaddle drawOn: aCanvas.
        gameState ball drawOn: aCanvas.
    ].
    
].

PongGameMorph new openInWindow.
OSWindow mainLoop.