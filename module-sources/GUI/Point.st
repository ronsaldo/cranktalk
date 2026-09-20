"Point accessing and construction"
Point ![
x
    ^ x
].

Point ![
y
    ^ y
].

Point class ![
x: x y: y
    ^ self basicNew
        setX: x setY: y;
        yourself
].

Point ![
setX: xValue setY: yValue 
    x := xValue.
    y := yValue.
].

Point ![
printOn: aStream
    aStream
        nextPut: $(;
        print: x;
        nextPut: $@;
        print: y;
        nextPut: $)
].

Number ![
@ other
    ^ Point x: self y: other
].

"Testing"
Object ![
isPoint
    ^ false
].

"Testing"
Point ![
isPoint
    ^ true
].

"Point arithmetic"
Point ![
+ arg
    ^ (x + arg x) @ (y + arg y)
].

Point ![
- arg
    ^ (x - arg x) @ (y - arg y)
].

Point ![
/ arg
    ^ (x / arg x) @ (y / arg y)
].

Point ![
// arg
    ^ (x // arg x) @ (y // arg y)
].

Point ![
<= aPoint
    ^ x <= aPoint x and: [y <= aPoint y]
].

"Stdio stdout print: (0@1) + (2 @ -3); lf."