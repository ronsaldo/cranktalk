Number ![
max: other
    ^ self >= other ifTrue: [self] ifFalse: [other]
].

Number ![
min: other
    ^ self <= other ifTrue: [self] ifFalse: [other]
].

Number ![
+ other
    self subclassResponsibility
].

SmallInteger ![
+ other
    <primitive: #'SmallInteger::+'>
    ^ super + other
].

Stdio stdout print: (1 perform: #negated); lf.
Stdio stdout print: (1 perform: #+ with: 2); lf.
Stdio stdout print: 1 + 2; lf.
Stdio stdout print: 1 + 2.0; lf.
