Number ![
max: other
    ^ self >= other ifTrue: [self] ifFalse: [other]
].

Number ![
min: other
    ^ self <= other ifTrue: [self] ifFalse: [other]
].

SmallInteger ![
+ other
    <primitive: #'SmallInteger::+'>
    self error: 'TODO: SmallInteger::+'
].

"Stdio stdout print: 1 + 2; lf."
"Stdio stdout print: 1 + 2.0; lf."
