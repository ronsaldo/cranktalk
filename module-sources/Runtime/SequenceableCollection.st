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
