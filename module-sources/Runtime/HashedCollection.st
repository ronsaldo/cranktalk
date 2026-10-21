HashedCollection ![
initialize
    super initialize.
    size := 0.
].

HashedCollection ![
capacity
    ^ storage == nil
        ifTrue: [0]
        ifFalse: [storage size]
].

HashedCollection ![
size
    ^ size
].

HashedCollection ![
increaseCapacity
    self subclassResponsibility
].

HashedCollection ![
scanFor: anObject
    self subclassResponsibility
].

HashedCollection ![
scanForOrIncreaseCapacity: anObject
    | index |
    index := self scanFor: anObject.
    index < 1 ifTrue: [
        self increaseCapacity.
        index := self scanFor: anObject.
        index < 1 ifTrue: [
            self error: 'Not space for hashed collection.'
        ]
    ].

    ^ index
].

