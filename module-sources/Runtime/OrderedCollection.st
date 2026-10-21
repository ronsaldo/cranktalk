OrderedCollection ![
capacity
    ^ storage == nil
        ifTrue: [0]
        ifFalse: [storage size]
].

OrderedCollection ![
initialize
    super initialize.
    size := 0.
].

OrderedCollection ![
initialize: initialCapacity
    self initialize.
    storage := Array basicNew: initialCapacity.
].

OrderedCollection class ![
new
    ^ self new: 10
].

OrderedCollection class ![
new: initialCapacity
    ^ self basicNew initialize: initialCapacity; yourself
].
