Dictionary ![
scanFor: anObject
    | capacity start finish |
    capacity := self capacity.
    capacity = 0 ifTrue: [^ 0].

    start := (anObject hash \\ capacity) + 1.
    finish := capacity.
    start to: finish do: [:i |
        | element |
        element := storage at: i.
        (element == nil or: [element key = anObject]) ifTrue: [
            ^ i
        ]
    ].

    1 to: start - 1 do: [:i |
        | element |
        element := storage at: i.
        (element == nil or: [element key = anObject]) ifTrue: [
            ^ i
        ]
    ].

    ^ 0
].

Dictionary ![
at: key put: anObject
    | index association |
    index := self scanForOrIncreaseCapacity: key.
    association := storage at: index.
    association == nil ifTrue: [
        association := Association key: key value: anObject.
        storage at: index put: association.
        size := size + 1.
    ] ifFalse:[
        association value: anObject
    ].
    ^ anObject
].

Dictionary ![
increaseCapacity
    | oldCapacity oldStorage newCapacity newStorage |
    oldCapacity := self capacity.
    oldStorage := storage.

    newCapacity := oldCapacity*2.
    newCapacity < 8 ifTrue: [
        newCapacity := 8.
    ].

    newStorage := Array basicNew: newCapacity.

    1 to: oldCapacity do: [:i |
        | oldAssociationValue |
        oldAssociationValue := oldStorage at: i.
        oldAssociationValue ~~ nil ifTrue: [
            self insertWithoutChecking: oldAssociationValue
        ]
    ].

    storage := newStorage
].

Dictionary ![
at: key
    ^ self at: key ifAbsent: [self error: 'Key not found']
].

Dictionary ![
at: key ifAbsent: aBlock
    | index association |
    index := self scanFor: key.
    index < 1 ifTrue: [^ aBlock value].

    association := storage at: index.
    association == nil ifTrue: [ ^ aBlock value].

    ^ association value
].


Dictionary ![
printOn: aStream
    | capacity outputCount|
    aStream print: self class.
    aStream nextPut: $(.

    capacity := self capacity.
    outputCount := 0.
    1 to: capacity do: [:index |
        | association |
        association := storage at: index.
        association ~~ nil ifTrue: [
            outputCount > 0 ifTrue: [
                aStream nextPut: $ .
            ].
            aStream print: association.
            outputCount := outputCount + 1
        ]
    ].

    aStream nextPut: $).
].
