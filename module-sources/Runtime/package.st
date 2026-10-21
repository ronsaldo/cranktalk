thisContext 
    loadFile: 'Array.st';
    loadFile: 'Association.st';
    loadFile: 'Behavior.st';
    loadFile: 'Class.st';
    loadFile: 'Metaclass.st';
    
    loadFile: 'Collection.st';
    loadFile: 'HashedCollection.st';
    loadFile: 'Dictionary.st';
    loadFile: 'IdentityDictionary.st';
    loadFile: 'MethodDictionary.st';
    loadFile: 'SequenceableCollection.st';
    loadFile: 'OrderedCollection.st';
    loadFile: 'Number.st'.

"Stdio stdout nextPutAll: 'Hello World from Runtime/package.st'; lf.
[
    self assert: true.
    self assert: true description: 'myAssert'.
] value.

Stdio stdout print: ({1. 2 . 3} collect: [:x | x * 2]); lf.
Stdio stdout print: ({1. 2 . 3} collectWithIndex: [:x :index | x * 2 + index]); lf.
Stdio stdout print: ({1. 2 . 3} select: [:x | x >= 2]); lf.
Stdio stdout print: ({1. 2 . 3} reject: [:x | x >= 2]); lf.
Stdio stdout print: {1. 2 . 3} = {1. 2. 3}; lf."