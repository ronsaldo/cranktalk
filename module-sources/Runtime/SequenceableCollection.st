SequenceableCollection ![
collect: aBlock
    | size array |
    size := self size.
    array := Array basicNew: size.
    1 to: size do: [:index |
        array basicAt: index put: (aBlock value: (self at: index))
    ].

    ^ array
].

SequenceableCollection ![
collectWithIndex: aBlock
    | size array |
    size := self size.
    array := Array basicNew: size.
    1 to: size do: [:index |
        array basicAt: index put: (aBlock value: (self at: index) value: index)
    ].

    ^ array
].

SequenceableCollection ![
reverseDo: aBlock
    1 to: self size do: [:index |
        aBlock value: (self at: self size - index + 1)
    ]
].

SequenceableCollection ![
do: aBlock
    1 to: self size do: [:index |
        aBlock value: (self at: index)
    ]
].

SequenceableCollection ![
do: aBlock separatedBy: aSeparator
    1 to: self size do: [:index |
        index > 1 ifTrue: [
            aSeparator value
        ].
        aBlock value: (self at: index)
    ]
].

SequenceableCollection ![
hasEqualElements: otherCollection
    | size |
    (otherCollection isKindOf: SequenceableCollection) ifFalse: [^ false].
    
    size := self size.
    size = otherCollection size ifFalse: [^ false ].
    1 to: size do: [:index |
        (self at: size) = (otherCollection at: size) ifFalse: [
            ^ false
        ]
    ].
    ^ true
].

SequenceableCollection ![
= otherCollection
    self == otherCollection ifTrue: [^ true].
    self species == otherCollection species ifFalse: [^ false ].
    ^ self hasEqualElements: otherCollection
].
