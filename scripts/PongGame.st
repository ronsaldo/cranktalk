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
velocity
    ^ velocity
].

PongGameEntity ![
velocity: newVelocity
    velocity := newVelocity
].

PongGameEntity ![
halfExtent
    ^ halfExtent
].

PongGameEntity ![
halfExtent: newHalfExtent
    halfExtent := newHalfExtent
].

PongGameEntity ![
color
    ^ color
].

PongGameEntity ![
color: newColor
    color := newColor
].

PongGameEntity ![
drawOn: aCanvas
    aCanvas fillRectangle: self bounds color: color
].

PongGameEntity ![
updateWithTimestep: delta
    position := position + (velocity * (delta@delta)).
].

"Wall"
PongGameEntity << #PongGameWall
    slots: {};
    build.

"Paddle"
PongGameEntity << #PongGamePaddle
    slots: {};
    build.

PongGamePaddle ![
initialize
    super initialize.
    halfExtent := 0.5@3.0
].

PongGamePaddle ![
updateWithTimestep: delta
    | height |
    position := position + (velocity * (delta@delta)).
    height := position y min: 12 - halfExtent y max: -12 + halfExtent y.
    position := position x @ height

].

"Ball"
PongGameEntity << #PongGameBall
    slots: {};
    build.

PongGameBall ![
initialize
    super initialize.
    halfExtent := 0.5@0.5.
].

PongGameBall ![
bounceVertical
    velocity := velocity x @ velocity y negated
].

PongGameBall ![
bounceHorizontal
    velocity := velocity x negated @ velocity y
].

"Game state"
Object << #PongGameState
    slots: {#firstPaddle. #secondPaddle. #ball. #topWall. #bottomWall. #leftWall. #rightWall. #player1Score. #player2Score};
    build.

PongGameState ![
initialize
    super initialize.
    firstPaddle := PongGamePaddle new.
    secondPaddle := PongGamePaddle new.
    ball := PongGameBall new.
    topWall := PongGameWall new.
    bottomWall := PongGameWall new.

    leftWall := PongGameWall new.
    rightWall := PongGameWall new.

    topWall position: 0@13; halfExtent: 20@1.
    bottomWall position: 0@ -13; halfExtent: 20@1.

    leftWall position: -20@0; halfExtent: 0.5@20; color: Color red.
    rightWall position: 20@0; halfExtent: 0.5@20; color: Color red.

    firstPaddle position: -19@0.0.
    secondPaddle position: 19@0.0.

    player1Score := 0.
    player2Score := 0.
    self resetBall.
].

PongGameState ![
resetBall
    ball position: 0@0; velocity: -10.0@10.0.
].

PongGameState ![
entitiesDo: aBlock
    aBlock value: leftWall.
    aBlock value: rightWall.

    aBlock value: topWall.
    aBlock value: bottomWall.

    aBlock value: firstPaddle.
    aBlock value: secondPaddle.
    aBlock value: ball.
].

PongGameState ![
topWall
    ^ topWall
].

PongGameState ![
bottomWall
    ^ bottomWall
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

    ((ball bounds intersects: topWall bounds) or: [(ball bounds intersects: bottomWall bounds)]) ifTrue: [
        ball bounceVertical
    ].

    ((ball bounds intersects: firstPaddle bounds) or: [(ball bounds intersects: secondPaddle bounds)]) ifTrue: [
        ball bounceHorizontal
    ].

    (ball bounds intersects: leftWall bounds) ifTrue: [
        player2Score := player2Score + 1.
        self resetBall
    ].

    (ball bounds intersects: rightWall bounds) ifTrue: [
        player1Score := player1Score + 1.
        self resetBall
    ].
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

    scaleFactor := self extent / (40.0@ -25.0).
    aCanvas withTranslation: self extent * (0.5 @ 0.5) do: [
        aCanvas withScaleFactor: scaleFactor do: [
            gameState entitiesDo: [:anEntity |
                anEntity drawOn: aCanvas.
            ].
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

PongGameMorph ![
onMouseMotionEvent: anEvent
    anEvent wasHandled ifTrue: [^ self].

    self takeKeyboardFocus.
    anEvent wasHandled: true
].

PongGameMorph ![
onKeyboardDownEvent: anEvent
    anEvent wasHandled ifTrue: [^ self].

    anEvent symbol == OSK_w ifTrue: [
        gameState firstPaddle velocity: 0 @ 20.
    ].

    anEvent symbol == OSK_s ifTrue: [
        gameState firstPaddle velocity: 0@ -20.
    ].

    anEvent symbol == OSK_UP ifTrue: [
        gameState secondPaddle velocity: 0 @ 20.
    ].

    anEvent symbol == OSK_DOWN ifTrue: [
        gameState secondPaddle velocity: 0@ -20.
    ].
].

PongGameMorph ![
onKeyboardUpEvent: anEvent
    anEvent wasHandled ifTrue: [^ self].

    anEvent symbol == OSK_w ifTrue: [
        gameState firstPaddle velocity y > 0 ifTrue: [gameState firstPaddle velocity: 0@0].
    ].

    anEvent symbol == OSK_s ifTrue: [
        gameState firstPaddle velocity y < 0 ifTrue: [gameState firstPaddle velocity: 0@0].
    ].

    anEvent symbol == OSK_UP ifTrue: [
        gameState secondPaddle velocity y > 0 ifTrue: [gameState secondPaddle velocity: 0@0].
    ].

    anEvent symbol == OSK_DOWN ifTrue: [
        gameState secondPaddle velocity y < 0 ifTrue: [gameState secondPaddle velocity: 0@0].
    ].
].

PongGameMorph new openInWindow.
OSWindow mainLoop.