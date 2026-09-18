thisContext loadFile: 'Collection.st'.
thisContext loadFile: 'SequenceableCollection.st'.
thisContext loadFile: 'Array.st'.

Stdio stdout nextPutAll: 'Hello World from Runtime/package.st'; lf.
[
    self assert: true.
    self assert: true description: 'myAssert'.
] value.

Stdio stdout print: ({1. 2 . 3} collect: [:x | x * 2]); lf.
Stdio stdout print: ({1. 2 . 3} collectWithIndex: [:x :index | x * 2 + index]); lf.
Stdio stdout print: ({1. 2 . 3} select: [:x | x >= 2]); lf.
Stdio stdout print: ({1. 2 . 3} reject: [:x | x >= 2]); lf.