Morph << #ImageMorph
    slots: {#form};
    build.

ImageMorph ![
drawOn: aCanvas
    form ~~ nil ifTrue: [
        aCanvas translucentImage: form at: 0@0
    ]
].

ImageMorph ![
fitSize
    form ~~ nil ifTrue: [
        self extent: form extent
    ]
].

ImageMorph ![
form
    ^ form
].

ImageMorph ![
form: aForm
    form := aForm.
    self fitSize.
].

Form ![
asMorph
    ^ ImageMorph new form: self; yourself
].
