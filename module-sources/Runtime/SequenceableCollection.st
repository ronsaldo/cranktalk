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
