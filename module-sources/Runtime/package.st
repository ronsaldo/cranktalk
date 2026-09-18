thisContext loadFile: 'Collection.st'.
thisContext loadFile: 'SequenceableCollection.st'.

Stdio stdout nextPutAll: 'Hello World from Runtime/package.st'; lf.
[
    self assert: true.
    self assert: true description: 'myAssert'.
] value.

Stdio stdout print: {1. 2}; lf