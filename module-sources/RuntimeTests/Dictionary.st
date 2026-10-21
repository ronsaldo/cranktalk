"Two elements dictionary"
[
    | dictionary |
    dictionary := Dictionary new.
    self assert: dictionary size = 0.

    dictionary at: #first put: 1.
    dictionary at: #second put: 2.
    
    self assert: dictionary size = 2.
    self assert: (dictionary at: #first) = 1.
    self assert: (dictionary at: #second) = 2.

    self assert: dictionary printString = 'Dictionary(#first->1 #second->2)'.
] value.
