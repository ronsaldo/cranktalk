"OrderedCollection"
[
    | list |
    list := OrderedCollection new.
    self assert: list capacity = 10.
    self assert: list size = 0.

    list add: 1; add: 2.
    self assert: list size = 2.

    self assert: list first = 1.
    self assert: list second = 2.
] value.