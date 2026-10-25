Random class ![
seed: aSeed
    ^ self new seed: aSeed; yourself
].

Random ![
initialize
    seed := 42.
].

Random ![
seed
    ^ seed
].

Random ![
seed: newSeed
    ^ seed
].

Random ![
maxValue
    ^ 16r7FFFFFFF
].

Random ![
next
    ^ self privateNext asFloat / self maxValue asFloat
].


Random ![
nextBetween: lowerBound and: upperBound
    ^ lowerBound + (self next * (upperBound - lowerBound))
].
