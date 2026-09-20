Morph << #BorderedMorph
    slots: {#borderColor. #borderWidth};
    build.

BorderedMorph ![
initialize
    super initialize.
    borderColor := self defaultBorderColor.
    borderWidth := self defaultBorderWidth.
].

BorderedMorph ![
defaultBorderColor
    ^ Color black    
].

BorderedMorph ![
defaultBorderWidth
    ^ 1    
].

BorderedMorph ![
drawOn: aCanvas
    super drawOn: aCanvas.
    aCanvas frameRectangle: self localBounds borderWidth: borderWidth color: borderColor
].
