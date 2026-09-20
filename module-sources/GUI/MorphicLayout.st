Object << #MorphicLayout
    slots: {};
    build.
Object << #FillMorphicLayout
    slots: {#morph};
    build.
Object << #CenterMorphicLayout
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

CenterMorphicLayout ![
applyWithLocalBounds: localBounds
    | childExtent localCenter | 
    childExtent := morph extent.
    localCenter := localBounds extent // (2@2).
    morph origin: localCenter - (childExtent // (2@2)).
].

CenterMorphicLayout ![
morph: aMorph
    morph := aMorph
].
