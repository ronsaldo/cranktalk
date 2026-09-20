Point ![
corner: aCorner
    ^ Rectangle origin: self corner: aCorner
].

Point ![
extent: anExtent
    ^ Rectangle origin: self corner: self + anExtent
].

Rectangle class ![
origin: anOrigin corner: aCorner
    ^ self basicNew setPoint: anOrigin point: aCorner
].

Rectangle ![
setPoint: anOrigin point: aCorner
    origin := anOrigin.
    corner := aCorner.
].

Rectangle ![
extent
    ^ corner - origin
].

Rectangle ![
corner
    ^ corner
].

Rectangle ![
origin
    ^ origin
].

Rectangle ![
containsPoint: aPoint
    ^ origin <= aPoint and: [aPoint <= corner]
].
