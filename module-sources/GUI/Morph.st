Object << #Morph
    slots: {#bounds . #owner . #submorphs . #color};
    build.

Morph ![
initialize
    super initialize.
    submorphs := Array new.
    bounds := self defaultBounds.
    color := self defaultColor.
].

Morph ![
defaultBounds
    ^ 0@0 corner: 50@40
].

Morph ![
defaultColor
    ^ Color blue
].
