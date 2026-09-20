BorderedMorph << #SimpleButtonMorph
    slots: {#focusColor. #clickColor. #hasButtonDown . #label. #onClickAction};
    build.

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