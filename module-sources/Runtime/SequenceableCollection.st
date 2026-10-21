SequenceableCollection ![
first
    ^ self at: 1
].

SequenceableCollection ![
second
    ^ self at: 2
].

SequenceableCollection ![
third
    ^ self at: 3
].

SequenceableCollection ![
fourth
    ^ self at: 4
].

SequenceableCollection ![
fifth
    ^ self at: 5
].

SequenceableCollection ![
sixth
    ^ self at: 6
].

SequenceableCollection ![
seventh
    ^ self at: 7
].

SequenceableCollection ![
eighth
    ^ self at: 8
].

SequenceableCollection ![
ninth
    ^ self at: 9
].

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
        (self at: index) = (otherCollection at: index) ifFalse: [
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

SequenceableCollection ![
atAllPut: anObject
    1 to: self size do: [:index | self at: index put: anObject]
].

SequenceableCollection ![
, otherCollection
    | mySize otherSize result |
    mySize := self size.
    otherSize := otherCollection size.
    result := self species new: mySize + otherSize.
    1 to: mySize do: [:i |
        result at: i put: (self at: i)
    ].
    1 to: otherSize do: [:i |
        result at: mySize + i put: (otherCollection at: i)
    ].
    ^ result
]