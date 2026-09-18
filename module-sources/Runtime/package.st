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

{1. 2} do: [:each |
    Stdio stdout print: each; lf
].