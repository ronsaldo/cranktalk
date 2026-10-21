Variable ![
key
    ^ name
].

Variable ![
name
    ^ name
].

LiteralVariable ![
value
    ^ value
].

LiteralVariable ![
value: newValue
    value := newValue
].

ClassVariable ![
printOn: aStream
    aStream print: name; nextPutAll: ' => '; print: value
].
