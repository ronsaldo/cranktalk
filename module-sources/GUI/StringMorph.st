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
    "By default do nothing"
].

StringMorph ![
label
    ^ label
].

StringMorph ![
label: aLabel
    label := aLabel
].
