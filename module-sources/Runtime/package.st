Stdio stdout nextPutAll: 'Hello World from Runtime/package.st'; lf.
[
    self assert: true.
    self assert: true description: 'myAssert'.
] value.

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

"{1. 2} do: [:each |
    Stdio stdout print: each; lf
]."

Array ![
printOn: aStream
    aStream nextPutAll: 'Array('.
    self do: [:value |
        aStream print: value
    ] separatedBy: [aStream space].

    aStream nextPutAll: ')'.
].

Stdio stdout print: {1. 2}; lf