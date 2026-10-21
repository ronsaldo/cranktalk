Class ![
subclasses
    ^ subclasses
].

Class ![
name
    ^ name
].

Class ![
classPool
    ^ classPool
].

Class ![
sharedPools
    ^ sharedPools
].

Class ![
environment
    ^ environment
].

Class ![
addSharedPool: aSharedPool
    sharedPools := sharedPools copyWith: aSharedPool
].
