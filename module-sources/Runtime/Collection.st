Collection ![
printOn: aStream
    super printOn: aStream.
    aStream nextPutAll: '('.
    self do: [:value |
        aStream print: value
    ] separatedBy: [aStream space].

    aStream nextPutAll: ')'.
].
