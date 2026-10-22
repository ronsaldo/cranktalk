"Pong game entity"
Object << #PongGameEntity
    slots: {#position . #velocity. #halfExtent. #color};
    build.

PongGameEntity ![
initialize
    super initialize.
    position := 0.0@0.0.
    velocity := 0.0@0.0.
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
    halfExtent := 0.5@3.0
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
    slots: {#firstPaddle. #secondPaddle. #ball. #player1Score. #player2Score};
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

    player1Score := 0.
    player2Score := 0.
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

PongGameState ![
player1Score
    ^ player1Score
].

PongGameState ![
player2Score
    ^ player2Score
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

    aCanvas
        drawString: gameState player1Score asString inRectangle: (10@10 extent: 10@10) color: Color white;
        drawString: gameState player2Score asString inRectangle: ((self extent x @ 0) + (-20@10) extent: 10@10) color: Color white.

].

PongGameMorph ![
updateAnimation
    Stdio stdout nextPutAll: 'updateAnimation'; lf.
].

PongGameMorph new openInWindow.
OSWindow mainLoop.