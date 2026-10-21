"Array atAllPut:"
[
    | array |
    array := (Array new: 3) atAllPut: 42; yourself.
    self assert: array first = 42.
    self assert: array second = 42.
    self assert: array third = 42.
] value.

"Array concat"
[
    | array |
    array := #(1 2) , #(3 4).
    self assert: array first = 1.
    self assert: array second = 2.
    self assert: array third = 3.
    self assert: array fourth = 4.
] value.
