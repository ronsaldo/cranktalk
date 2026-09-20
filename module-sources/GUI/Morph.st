Object << #Morph
    slots: {#bounds . #owner . #submorphs . #color};
    build.

Morph ![
initialize
    super initialize.
    submorphs := OrderedCollection new.
    bounds := self defaultBounds.
    color := self defaultColor.
].

Morph ![
addMorph: aMorph
    submorphs add: aMorph
].

Morph ![
defaultBounds
    ^ 0@0 corner: 50@40
].

Morph ![
defaultColor
    ^ Color blue
].

Morph ![
extent
    ^ bounds extent
].

Morph ![
extent: anExtent
    bounds := bounds origin extent: anExtent
].