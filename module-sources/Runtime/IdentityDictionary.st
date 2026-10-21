IdentityDictionary ![
scanFor: anObject
    | capacity start finish |
    capacity := self capacity.
    capacity = 0 ifTrue: [^ 0].

    start := (anObject identityHash \\ capacity) + 1.
    finish := capacity.
    start to: finish do: [:i |
        | element |
        element := storage at: i.
        (element == nil or: [element key == anObject]) ifTrue: [
            ^ i
        ]
    ].

    1 to: start - 1 do: [:i |
        | element |
        element := storage at: i.
        (element == nil or: [element key == anObject]) ifTrue: [
            ^ i
        ]
    ].

    ^ 0
].
