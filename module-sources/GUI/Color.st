Color class ![
r: r g: g b: b a: a
    ^ self basicNew setR: r g: g b: b a: a
].

Color class ![
blue
    ^ self r: 0.0 g: 0.0 b: 1.0 a: 1.0
].
Color ![
setR: initRed g: initGreen b: initBlue a: initAlpha
    r := initRed.
    g := initGreen.
    b := initBlue.
    a := initAlpha.
].
