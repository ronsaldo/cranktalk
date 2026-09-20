BorderedMorph << #SimpleButtonMorph
    slots: {#focusColor. #clickColor. #hasButtonDown . #label. #onClickAction};
    build.
SimpleButtonMorph ![
initialize
    super initialize.
    color := Color r: 0.7 g: 0.7 b: 0.7.
    focusColor := Color r: 0.8 g: 0.8 b: 0.8.
    clickColor := Color r: 0.9 g: 0.9 b: 0.9.
    hasButtonDown := false.
].

SimpleButtonMorph ![
currentColor
    hasButtonDown ifTrue: [^ clickColor].
    self hasMouseFocus ifTrue: [^ focusColor].
    ^ color
].


SimpleButtonMorph ![
label: aLabelText
    label := StringMorph new
        label: aLabelText;
        fitSize;
        yourself.
    self addMorph: label.

    self extent: label extent + (20@20).
    self layout: (CenterMorphicLayout new
        morph: label;
        yourself).
]