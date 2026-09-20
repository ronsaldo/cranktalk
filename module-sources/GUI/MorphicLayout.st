Object << #MorphicLayout
    slots: {};
    build.
Object << #FillMorphicLayout
    slots: {#morph};
    build.


MorphicLayout ![
applyWithLocalBounds: localBounds
    self subclassResponsibility
].

FillMorphicLayout ![
applyWithLocalBounds: localBounds
    morph bounds: localBounds
].

FillMorphicLayout ![
morph: aMorph
    morph := aMorph
].

