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
label
    ^ label
].

StringMorph ![
label: aLabel
    label := aLabel
].
