CompiledFunction ![
name
    ^ name
].

CompiledFunction ![
owner
    ^ owner
].

CompiledFunction ![
primitiveName
    ^ primitiveName
].

CompiledFunction ![
printOn: aStream
    aStream print: owner; nextPutAll: '>>'; print: name
].
