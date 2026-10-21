Association class ![
key: key value: value
    ^ self basicNew key: key; value: value; yourself
].

Object ![
-> anObject
    ^ Association key: self value: anObject
].

LookupKey ![
key
    ^ key
].

LookupKey ![
key: newKey
    key := newKey
].

Association ![
value
    ^ value
].

Association ![
value: newValue
    value := newValue
].


Association ![
printOn: aStream
    aStream print: key;  nextPutAll: '->'; print: value.
].