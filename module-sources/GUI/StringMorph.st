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
