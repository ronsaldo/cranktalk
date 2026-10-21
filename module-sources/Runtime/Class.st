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

Class ![
addClassVarNamed: aStringOrSymbol
    self addClassVariable: (aStringOrSymbol asSymbol asClassVariable)
].

Class ![
addClassVariable: aClassVariable
    self classPool add: aClassVariable
].

Class ![
writeClassVariableNamed: aSymbol value: anObject
    (self classPool associationAt: aSymbol) value: anObject
].

