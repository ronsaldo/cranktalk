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

PongGameEntity ![
updateWithTimestep: delta
    position := position + (velocity * (delta@delta)).
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
    halfExtent := 0.5@0.5.
    velocity := -15.0@15.0.
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

    firstPaddle position: -19@0.0.
    secondPaddle position: 19@0.0.
    ball position: 0@0.

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

PongGameState ![
updateWithTimestep: delta
    firstPaddle updateWithTimestep: delta.
    secondPaddle updateWithTimestep: delta.
    ball updateWithTimestep: delta.
].

"Game morph"
Morph << #PongGameMorph
    slots: {#gameState . #lastAnimationTime. #accumulatedTime};
    build.

PongGameMorph ![
initialize
    super initialize.
    gameState := PongGameState new.
    accumulatedTime := 0.0.
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
    aCanvas withTranslation: self extent * (0.5 @ 0.5) do: [
        aCanvas withScaleFactor: scaleFactor do: [
            gameState firstPaddle drawOn: aCanvas.
            gameState secondPaddle drawOn: aCanvas.
            gameState ball drawOn: aCanvas.
        ].
    ].

    aCanvas
        drawString: gameState player1Score asString inRectangle: (10@10 extent: 10@10) color: Color white;
        drawString: gameState player2Score asString inRectangle: ((self extent x @ 0) + (-20@10) extent: 10@10) color: Color white.

].

PongGameMorph ![
updateWithTimestep: delta
    gameState updateWithTimestep: delta
].

PongGameMorph ![
updateAnimation
    | newTime deltaTime deltaTimeSeconds timeStep |
    lastAnimationTime == nil ifTrue: [lastAnimationTime := Time microsecondClockValue].
    newTime := Time microsecondClockValue.

    deltaTime := newTime - lastAnimationTime.
    lastAnimationTime := newTime.

    "Accumulate time"
    deltaTimeSeconds := deltaTime asFloat * 0.000001.
    accumulatedTime := accumulatedTime + deltaTimeSeconds.
    timeStep := 1.0 / 120.0.

    "Handle pauses"
    accumulatedTime >= (timeStep * 8) ifTrue: [
        accumulatedTime := 0.0
    ].

    "Simulate the timesteps"
    [accumulatedTime >= timeStep] whileTrue: [
        self updateWithTimestep: timeStep.
        accumulatedTime := accumulatedTime - timeStep
    ].

    self changed
].

PongGameMorph new openInWindow.
OSWindow mainLoop.