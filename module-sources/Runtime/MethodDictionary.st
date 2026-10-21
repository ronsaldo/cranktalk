MethodDictionary ![
capacity
    ^ super capacity // 2
].

MethodDictionary ![
scanFor: anObject
    | capacity start finish |
    capacity := self capacity.
    capacity = 0 ifTrue: [^ 0].

    start := (anObject identityHash \\ capacity) + 1.
    finish := capacity.
    start to: finish do: [:i |
        | key |
        key := storage at: i*2 - 1.
        (key == nil or: [key == anObject]) ifTrue: [
            ^ i
        ]
    ].

    1 to: start - 1 do: [:i |
        | key |
        key := storage at: i*2 - 1.
        (key == nil or: [key == anObject]) ifTrue: [
            ^ i
        ]
    ].

    ^ 0
].

MethodDictionary ![
atOrNil: aKey
    | index |
    index := self scanFor: aKey.
    index < 1 ifTrue: [ ^ nil ].

    ^ storage at: index*2
].

MethodDictionary ![
at: aKey ifAbsent: aBlock
    | index keyValue |
    index := self scanFor: aKey.
    index < 1 ifTrue: [ ^ aBlock value ].

    keyValue := index*2 - 1.
    keyValue == nil ifTrue: [^ aBlock value].

    ^ storage at: index*2
].

MethodDictionary ![
at: aKey
    ^ self at: aKey ifAbsent: [self error: 'Key is not found']
].

MethodDictionary ![
associationsDo: aBlock
    | capacity |
    capacity := self capacity.
    1 to: capacity do: [:i |
        | key |
        key := storage at: i*2 - 1.
        key ~~ nil ifTrue: [
            | value |
            value := storage at: i*2.
            aBlock value: (Association key: key value: value)
        ]
    ].
].

MethodDictionary ![
associations
    | result |
    result := OrderedCollection new.
    self associationsDo: [:key | result add: key]
].

MethodDictionary ![
keysDo: aBlock
    | capacity |
    capacity := self capacity.
    1 to: capacity do: [:i |
        | key |
        key := storage at: i*2 - 1.
        key ~~ nil ifTrue: [
            aBlock value: key
        ]
    ].
].

MethodDictionary ![
keys
    | result |
    result := OrderedCollection new.
    self keysDo: [:key | result add: key].
    ^ result
].

MethodDictionary ![
valuesDo: aBlock
    | capacity |
    capacity := self capacity.
    1 to: capacity do: [:i |
        | key |
        key := storage at: i*2 - 1.
        key ~~ nil ifTrue: [
            aBlock value: (storage at: i*2)
        ]
    ].
].

MethodDictionary ![
values
    | result |
    result := OrderedCollection new.
    self valuesDo: [:key | result add: key].
    ^ result
].

MethodDictionary ![
printOn: aStream
    | capacity outputCount |
    capacity := self capacity.
    aStream nextPutAll: 'MethodDictionary('.
    outputCount := 0.
    1 to: capacity do: [:index |
        | key |
        key := storage at: index * 2 - 1.
        (key ~~ nil) ifTrue: [
            | value |
            value := storage at: index * 2.
            outputCount > 0 ifTrue: [
                aStream nextPut: $ .
            ].
            aStream print: key; nextPutAll: '->'; print: value.
            outputCount := outputCount + 1
        ]
    ].
    aStream nextPut: $).
].
