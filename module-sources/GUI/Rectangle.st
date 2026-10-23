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

Rectangle ![
intersects: aRectangle
    "Method from Pharo"
    | rOrigin rCorner |
	rOrigin := aRectangle origin.
	rCorner := aRectangle corner.
	rCorner x <= origin x ifTrue: [ ^ false ].
	rCorner y <= origin y ifTrue: [ ^ false ].
	rOrigin x >= corner x ifTrue: [ ^ false ].
	rOrigin y >= corner y ifTrue: [ ^ false ].
	^ true
].
