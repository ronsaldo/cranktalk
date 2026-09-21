Number ![
max: other
    ^ self >= other ifTrue: [self] ifFalse: [other]
].

Number ![
min: other
    ^ self <= other ifTrue: [self] ifFalse: [other]
].