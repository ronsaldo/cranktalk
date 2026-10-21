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

Array ![
copyWith: anElement
    | size result |
    size := self basicSize.
    result := self class basicNew: size + 1.
    1 to: size do: [:i |
        result at: i put: (self basicAt: i)
    ].
    result at: size + 1 put: anElement.

    ^ result
]
