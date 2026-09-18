Array ![
select: aBlock
    | selectedValues |
    selectedValues := OrderedCollection new.
    1 to: self size do: [:index |
        | element |
        element := self at: index.
        (aBlock value: element) ifTrue: [
            selectedValues add: element
        ]
    ].
    ^ selectedValues asArray
].

Array ![
reject: aBlock
    | selectedValues |
    selectedValues := OrderedCollection new.
    1 to: self size do: [:index |
        | element |
        element := self at: index.
        (aBlock value: element) ifFalse: [
            selectedValues add: element
        ]
    ].
    ^ selectedValues asArray
].
