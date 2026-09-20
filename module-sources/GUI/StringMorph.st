Morph << #StringMorph
    slots: {#label};
    build.

StringMorph ![
initialize
    super initialize.
    label := ''.
].

StringMorph ![
defaultColor
    ^ Color black    
].

StringMorph ![
drawOn: aCanvas
    aCanvas drawString: label inRectangle: self localBounds color: color
].

StringMorph ![
fitSize
    self extent: (FontFace default measureTextExtent: label)
].

StringMorph ![
label
    ^ label
].

StringMorph ![
label: aLabel
    label := aLabel
].
