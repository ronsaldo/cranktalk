Color class ![
r: r g: g b: b a: a
    ^ self basicNew setR: r g: g b: b a: a
].

Color class ![
r: r g: g b: b
    ^ self r: r g: g b: b a: 1.0
].

Color class ![
black
    ^ self r: 0.0 g: 0.0 b: 0.0 a: 1.0
].

Color class ![
blue
    ^ self r: 0.0 g: 0.0 b: 1.0 a: 1.0
].

Color class ![
green
    ^ self r: 0.0 g: 1.0 b: 0.0 a: 1.0
].

Color class ![
red
    ^ self r: 1.0 g: 0.0 b: 0.0 a: 1.0
].

Color class ![
cyan
    ^ self r: 0.0 g: 1.0 b: 1.0 a: 1.0
].

Color class ![
magenta
    ^ self r: 1.0 g: 0.0 b: 1.0 a: 1.0
].

Color class ![
yellow
    ^ self r: 1.0 g: 1.0 b: 0.0 a: 1.0
].

Color class ![
white
    ^ self r: 1.0 g: 1.0 b: 1.0 a: 1.0
].

Color ![
setR: initRed g: initGreen b: initBlue a: initAlpha
    r := initRed.
    g := initGreen.
    b := initBlue.
    a := initAlpha.
].

Color ![
printOn: stream
    stream
    nextPutAll: 'Color r: '; print: r;
    nextPutAll: ' g: '; print: g;
    nextPutAll: ' b: '; print: b;
    nextPutAll: ' a: '; print: a
].
