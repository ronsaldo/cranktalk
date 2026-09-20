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
    self assert: aMorph owner == nil.
    submorphs add: aMorph.
    aMorph owner: self
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
localBounds
    ^ 0@0 corner: self extent
].

Morph ![
extent
    ^ bounds extent
].

Morph ![
extent: anExtent
    bounds := bounds origin extent: anExtent
].

Morph ![
owner
    ^ owner
].

Morph ![
owner: anOwner
    owner := anOwner
].

Morph ![
fullDrawOn: aCanvas
    aCanvas withTranslation: bounds origin do: [
        self drawOn: aCanvas.
        self drawChildrenWith: aCanvas
    ]
].

Morph ![
drawOn: aCanvas
    aCanvas fillRectangle: self localBounds color: color
].

Morph ![
drawChildrenWith: aCanvas
    submorphs do: [:each |
        each fullDrawOn: aCanvas
    ]
].